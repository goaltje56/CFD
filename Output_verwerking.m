clc
close all
clear all

output = load('output.dat');
nx = 20;
ny = 40;
%% columns
x       = output([ny+1:end],1);
y       = output([ny+1:end],2);
u       = output([ny+1:end],3);
v       = output([ny+1:end],4);
p       = output([ny+1:end],5);
T       = output([ny+1:end],6);
rho     = output([ny+1:end],7);
mu      = output([ny+1:end],8);
gamma   = output([ny+1:end],9);
k       = output([ny+1:end],10);
eps     = output([ny+1:end],11);
uplus   = output([ny+1:end],12);
yplus   = output([ny+1:end],13);
yplus1  = output([ny+1:end],14);
yplus2  = output([ny+1:end],15);
tw      = output([ny+1:end],16);
twx      = output([ny+1:end],17);


xmat = zeros(nx,1);
for i = 1:nx;
    xmat(i) = x(i*ny);
end
ymat = y(1:ny);

umat        = reshape(u,[ny,nx]);
vmat        = reshape(v,[ny,nx]);
pmat        = reshape(p,[ny,nx]);
Tmat        = reshape(T,[ny,nx]);
rhomat      = reshape(rho,[ny,nx]);
mumat       = reshape(mu,[ny,nx]);
gammamat    = reshape(gamma,[ny,nx]);
kmat        = reshape(k,[ny,nx]);
epsmat      = reshape(eps,[ny,nx]);
uplusmat    = reshape(uplus,[ny,nx]);
yplusmat    = reshape(yplus,[ny,nx]);
yplus1mat   = reshape(yplus1,[ny,nx]);
yplus2mat   = reshape(yplus2,[ny,nx]);
twmat      = reshape(tw,[ny,nx]);
twxmat      = reshape(twx,[ny,nx]);

velmag = sqrt(umat.^2 + vmat.^2);

figure(1)
subplot(2,1,1)
contourf(xmat,ymat,kmat,30 )
grid on
grid minor
shading interp
colorbar
subplot(2,1,2)
contourf(xmat,ymat,Tmat,30 )
grid on
shading interp
colorbar

R=0;
R2=0.1;
r=linspace(0,0.1,1000);

uT=1*(1-r/0.1).^(1/7);
uT2 = (r/0.1).^(1/7);

figure(2)
plot(ymat(:,1),umat(:,1))
hold on
plot(ymat(:,1),umat(:,end/2))
plot(ymat(:,1),umat(:,end))
plot(r+0.1,uT, 'k')
plot(r,uT2, 'k')
title('velocity in axial direction [m/s]')
legend('Entrance vel','midway vel','exit vel','analytical')

figure(3)
plot(xmat(:,1),velmag(end/2,:))
title('Centerline velocity magnitude [m/s]')
% for i=1:nx
%     for j=1:ny
% twtest(i,j) = mu(i,j) * 0.5*(u(i,j)+u(i+1,j))...
%     /(y-y);
%     end
% end
