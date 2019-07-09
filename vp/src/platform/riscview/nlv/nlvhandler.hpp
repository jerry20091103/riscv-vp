#pragma once

#include "elegantEnums.hpp"

#include <string>
#include <list>
#include <functional>

namespace nlv
{
DECLARE_ENUM(SType,
		EXT,
		DEF,
		BOX,
		HIERBOX);

DECLARE_ENUM(Direction,
		IN,
		OUT,
		INOUT);

class NLElement
{
public:
	virtual ~NLElement();
	virtual std::list<std::string> load() = 0;
	virtual std::list<std::string> update();
};


class Connectable
{
public:
	enum class Type
	{
		pin,
		port,
		hierPin,
	};
private:
	Type type;
public:
	Connectable(Type type);
	virtual ~Connectable();

	virtual std::string getName() = 0;
	Type getType();
};

class Port : public Connectable, public NLElement
{
	std::string name;
	Direction direction;
public:
	Port(std::string name, Direction direction);
	std::string getName();
	Direction& getDirection();

	std::list<std::string> load() override;
};

class Pin : public NLElement
{
	std::string name;
	Direction direction;
public:
	Pin(std::string name, Direction direction);
	std::string getName();
	Direction& getDirection();

	std::list<std::string> load() override;
};

class Instance;
class Symbol : public NLElement
{
	std::string name;
	std::string viewname;
	SType stype;
	std::vector<Pin> pins;
public:
	Symbol(std::string name, std::string viewname, SType stype);
	Symbol(std::string name, std::string viewname, SType stype, std::vector<Pin> pins);

	std::string getName();
	std::string getViewname();
	SType getStype();
	std::vector<Pin>& getPins();

	std::list<std::string> load() override;
	Instance instantiate(std::string name);
};

class Instance;
class PinInstance : public Connectable
{
	Pin& pin;
	Instance& instance;
public:
	PinInstance(Pin& pin, Instance& instance);
	Pin& getPin();
	std::string getName() override;
	Instance& getInstance();
};

class Instance : public NLElement
{
	std::string name;
	Symbol& symbol;
	std::string viewname;
	std::map<std::string, PinInstance*> pins;
	std::string text;
public:
	Instance(std::string name, std::string viewname, Symbol& symbol);

	std::string getName();
	Symbol& getSymbol();
	std::string getViewname();
	PinInstance* getPin(Pin& pin);
	void setText(std::string& text);

	std::list<std::string> load() override;
};

class Connection : public NLElement
{
	std::string name;
	std::vector<Connectable*> connectables;

public:
	Connection(std::string name);
	Connection(std::string name, std::vector<Connectable*> list);

	void add(Connectable* element);

	std::list<std::string> load() override;
};

} //namespace nlv

class NLVhandler
{
	std::function<bool(const char*)> command = nullptr;

public:
	NLVhandler(std::function<bool(const char*)> command);

	void init();

	bool add(nlv::NLElement& elem);

	void show();
};
