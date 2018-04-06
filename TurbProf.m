% clear all; close all; clc;
R=0;
R2=0.1;
r=linspace(0,0.1,1000);

uT=1*(1-r/0.1).^(1/4);
uT2 = (r/0.1).^(1/4);
figure(2)
hold on
plot(r+0.1,uT)
plot(r,uT2)