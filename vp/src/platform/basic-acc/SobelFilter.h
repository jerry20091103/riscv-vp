#ifndef SOBEL_FILTER_H_
#define SOBEL_FILTER_H_
#include <cmath>
#include <cstring>  // memcpy
#include <iomanip>
#include <systemc>

#include "filter_def.h"
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

using namespace sc_core;

class SobelFilter : public sc_module {
   public:
	tlm_utils::simple_target_socket<SobelFilter> t_skt;

	sc_fifo<unsigned char> i_r;
	sc_fifo<unsigned char> i_g;
	sc_fifo<unsigned char> i_b;
	sc_fifo<int> o_r;
	sc_fifo<int> o_g;
	sc_fifo<int> o_b;

	SC_HAS_PROCESS(SobelFilter);
	SobelFilter(sc_module_name n) : sc_module(n), t_skt("t_skt"), base_offset(0) {
		SC_THREAD(do_filter);
		t_skt.register_b_transport(this, &SobelFilter::blocking_transport);
	}
	~SobelFilter() = default;

   private:
	int median(unsigned char *a, int n) {
		unsigned char temp;
		for (int i = 0; i < n - 1; ++i) {
			for (int j = i + 1; j < n; ++j) {
				if (a[i] > a[j]) {
					temp = a[i];
					a[i] = a[j];
					a[j] = temp;
				}
			}
		}
		return a[n / 2];
	}

	void do_filter() {
		while (true) {
			// fill in the input buffer with the rightmost column of pixels
			for (unsigned int v = 0; v < MASK_Y; ++v) {
				for (unsigned int u = 0; u < MASK_X - 1; ++u) {
					input_buf[2][u][v] = input_buf[2][u + 1][v];
					input_buf[1][u][v] = input_buf[1][u + 1][v];
					input_buf[0][u][v] = input_buf[0][u + 1][v];
				}
				unsigned char r = i_r.read();
				unsigned char g = i_g.read();
				unsigned char b = i_b.read();
				input_buf[2][MASK_X - 1][v] = r;
				input_buf[1][MASK_X - 1][v] = g;
				input_buf[0][MASK_X - 1][v] = b;
			}

			for (unsigned int c = 0; c < 3; ++c)  // r, g, b 3 channels
			{
				// *apply median filter
				// find median value
				unsigned char temp[MASK_X * MASK_Y];
				memcpy(temp, input_buf[c], MASK_X * MASK_Y);
				int med = median(temp, MASK_X * MASK_Y);
				// replace the center pixel with the median value
				input_buf[c][MASK_X / 2][MASK_Y / 2] = med;

				// * apply mean filter
				val[c] = 0;
				for (unsigned int v = 0; v < MASK_Y; ++v) {
					for (unsigned int u = 0; u < MASK_X; ++u) {
						val[c] += mask[u][v] * input_buf[c][u][v];
					}
				}
				val[c] /= 10;  // divide by 10 to get the mean value
			}
			o_r.write(val[2]);
			o_g.write(val[1]);
			o_b.write(val[0]);
		}
	}
	int val[3];
	// a 9 pixel input buffer
	unsigned char input_buf[3][MASK_X][MASK_Y];

	// mean mask
	const int mask[MASK_X][MASK_Y] = {{1, 1, 1}, {1, 2, 1}, {1, 1, 1}};

	unsigned int base_offset;

	void blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay) {
		wait(delay);
		// unsigned char *mask_ptr = payload.get_byte_enable_ptr();
		// auto len = payload.get_data_length();
		tlm::tlm_command cmd = payload.get_command();
		sc_dt::uint64 addr = payload.get_address();
		unsigned char *data_ptr = payload.get_data_ptr();

		addr -= base_offset;

		// cout << (int)data_ptr[0] << endl;
		// cout << (int)data_ptr[1] << endl;
		// cout << (int)data_ptr[2] << endl;
		word buffer;

		switch (cmd) {
			case tlm::TLM_READ_COMMAND:
				// cout << "READ" << endl;
				switch (addr) {
					case SOBEL_FILTER_RESULT_ADDR:
						buffer.uc[0] = o_r.read();
						buffer.uc[1] = o_g.read();
						buffer.uc[2] = o_b.read();
						break;
					default:
						std::cerr << "READ Error! SobelFilter::blocking_transport: address 0x" << std::setfill('0')
						          << std::setw(8) << std::hex << addr << std::dec << " is not valid" << std::endl;
				}
				data_ptr[0] = buffer.uc[0];
				data_ptr[1] = buffer.uc[1];
				data_ptr[2] = buffer.uc[2];
				data_ptr[3] = buffer.uc[3];
				break;
			case tlm::TLM_WRITE_COMMAND:
				// cout << "WRITE" << endl;
				switch (addr) {
					case SOBEL_FILTER_R_ADDR:
						i_r.write(data_ptr[0]);
						i_g.write(data_ptr[1]);
						i_b.write(data_ptr[2]);
						break;
					default:
						std::cerr << "WRITE Error! SobelFilter::blocking_transport: address 0x" << std::setfill('0')
						          << std::setw(8) << std::hex << addr << std::dec << " is not valid" << std::endl;
				}
				break;
			case tlm::TLM_IGNORE_COMMAND:
				payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
				return;
			default:
				payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
				return;
		}
		payload.set_response_status(tlm::TLM_OK_RESPONSE);  // Always OK
	}
};
#endif
