clear all; close all; clc;
Rmax=0.01;
Rmax2 = .02;
uMax =1;
r=linspace(0,Rmax2,1000);

for i=1:500

    uT(i)=1*((r(i)/Rmax)).^(1/7);
end
for i = 500:1000

    uT(i)=1*(1- r(i)/Rmax2).^(1/7)+ (1-(1-Rmax/Rmax2)^(1/7));     
   
end

% uT=uMax*(r/0.02).^(1/17);
figure()
plot(r,uT)