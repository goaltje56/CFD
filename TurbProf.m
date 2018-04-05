% clear all; close all; clc;
R=0;
R2=0.01;
r=linspace(0,0.01,1000);
h=R2-R;
y=0.01-r;

uT=1*(1-r/0.01).^(1/7);
uT2 = (r/0.01).^(1/7);
figure(2)
hold on
plot(r+0.01,uT)
% plot(r,uT2)