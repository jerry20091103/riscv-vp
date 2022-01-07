# plot the generated data from the testbench/temp_source systemc ams simulation
clear all;
close all;
myXTicks = 1;
myYTicks = 2;
tb_data = load("-ascii","testbench.dat");
figure;
plot(tb_data(:,1),tb_data(:,2),'b-', "linewidth", 2);
## setpoints
line([0 5], [20 20], "linestyle", "-.", "color", "r", "linewidth", 2);
line([5 10], [10 10], "linestyle", "-.", "color", "r", "linewidth", 2);
line([0 0], [0 20], "linestyle", "-.", "color", "r", "linewidth", 2);
line([5 5], [20 10], "linestyle", "-.", "color", "r", "linewidth", 2);
line([10 10], [10 0], "linestyle", "-.", "color", "r", "linewidth", 2);
## margins
line([0 5], [20+1 20+1], "linestyle", "--", "color", "m", "linewidth", 2);
line([0 5], [20-1 20-1], "linestyle", "--", "color", "m", "linewidth", 2);
line([5 10], [10+1 10+1], "linestyle", "--", "color", "m", "linewidth", 2);
line([5 10], [10-1 10-1], "linestyle", "--", "color", "m", "linewidth", 2);
#title("TDF Environment Model \nTemperature data over time", "fontsize", 14);
axis([-0.5 11 -0.5 23]);
set(gca,'XTick',0:myXTicks:11)
set(gca,'YTick',0:myYTicks:23)
set(gca, "linewidth", 1, 'fontsize',18)
xlabel("t/sec",'fontsize',20);
ylabel("T/°C",'fontsize',20);
grid on;
figure;
plot(tb_data(:,1),tb_data(:,4),'b-',"linewidth", 2);
## setpoints
line([0 5], [20 20], "linestyle", "-.", "color", "r", "linewidth", 2);
line([5 10], [10 10], "linestyle", "-.", "color", "r", "linewidth", 2);
line([0 0], [0 20], "linestyle", "-.", "color", "r", "linewidth", 2);
line([5 5], [20 10], "linestyle", "-.", "color", "r", "linewidth", 2);
line([10 10], [10 0], "linestyle", "-.", "color", "r", "linewidth", 2);
## margins
line([0 5], [20+1 20+1], "linestyle", "--", "color", "m", "linewidth", 2);
line([0 5], [20-1 20-1], "linestyle", "--", "color", "m", "linewidth", 2);
line([5 10], [10+1 10+1], "linestyle", "--", "color", "m", "linewidth", 2);
line([5 10], [10-1 10-1], "linestyle", "--", "color", "m", "linewidth", 2);
#title("TDF Sensor Output \n Temperature data over time","fontsize", 14);
axis([-0.5 11 -0.5 23]);
set(gca,'XTick',0:myXTicks:11)
set(gca,'YTick',0:myYTicks:23)
set(gca, "linewidth", 1, 'fontsize',18)
xlabel("t/sec",'fontsize',20);
ylabel("T/°C",'fontsize',20);
grid on;

## combined plot
figure;
plot(tb_data(:,1),tb_data(:,4),'b-',"linewidth", 2,tb_data(:,1),tb_data(:,2),'k-',"linewidth", 2);
h=legend("TDF Sensor","TDF Environment Model");
set(h, 'fontsize', 14)
## setpoints
line([0 5], [20 20], "linestyle", "-.", "color", "r", "linewidth", 2);
line([5 10], [10 10], "linestyle", "-.", "color", "r", "linewidth", 2);
line([0 0], [0 20], "linestyle", "-.", "color", "r", "linewidth", 2);
line([5 5], [20 10], "linestyle", "-.", "color", "r", "linewidth", 2);
line([10 10], [10 0], "linestyle", "-.", "color", "r", "linewidth", 2);
## margins
line([0 5], [20+1 20+1], "linestyle", "--", "color", "m", "linewidth", 2);
line([0 5], [20-1 20-1], "linestyle", "--", "color", "m", "linewidth", 2);
line([5 10], [10+1 10+1], "linestyle", "--", "color", "m", "linewidth", 2);
line([5 10], [10-1 10-1], "linestyle", "--", "color", "m", "linewidth", 2);
#title("TDF Environment Model and TDF Sensor Output \n Temperature data over time");
axis([-0.5 11 -0.5 23]);
set(gca,'XTick',0:myXTicks:11)
set(gca,'YTick',0:myYTicks:23)
set(gca, "linewidth", 1, 'fontsize',18)
xlabel("t/sec",'fontsize',20);
ylabel("T/°C",'fontsize',20);
grid on;