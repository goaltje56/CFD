clc
close all
clear all

factor = [1 3 6];
% pp = [];
% max_v = [];
% max_v_analytical = [];
for a = 1:length(factor)

name = sprintf('U0.5_Ti0_0%1.0f_NoBluff.dat',factor(a))

output = load(name);
nx = 40;
ny = 80;
%% columns
x       = output([ny+1:end],1);
y       = output([ny+1:end],2);
u       = output([ny+1:end],3);
v       = output([ny+1:end],4);
p       = output([ny+1:end],5);
f       = output([ny+1:end],6);
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
mueff      = output([ny+1:end],18);
% T      = output([ny+1:end],19);


for ii = 1:length(u)
    vel_mag(ii) = sqrt(v(ii)^2+u(ii)^2);
    Re(:,ii) = rho(ii)*vel_mag(ii)*0.2/mu(ii);
    I(:,ii) = 0.16*Re(ii)^(-1/8);
end


xmat = zeros(nx,1);
for i = 1:nx;
    xmat(i) = x(i*ny);
end
ymat = y(1:ny);

umat        = reshape(u,[ny,nx]);
vmat        = reshape(v,[ny,nx]);
pmat        = reshape(p,[ny,nx]);
fmat        = reshape(f,[ny,nx]);
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
Remat       = reshape(Re,[ny,nx]);
mueffmat      = reshape(mueff,[ny,nx]);
Imat       = reshape(I,[ny,nx]);
vel_magmat       = reshape(vel_mag,[ny,nx]);
% Tmat         = reshape(T,[ny,nx]);


velmag = sqrt(umat.^2 + vmat.^2);

figure(1)
subplot(2,1,1)
contourf(xmat,ymat,umat,30 )
grid on
grid minor
shading interp
colorbar
subplot(2,1,2)
contourf(xmat,ymat,vmat,30 )
grid on
shading interp
colorbar


figure(2)
hold on
plot(ymat(:,1),fmat(:,end),'LineWidth',2)
set(gca, 'box', 'on', 'LineWidth', 2, 'FontSize', 15)
title('Mixture fraction as function of height', 'FontSize', 15)
ylabel('mixture fraction [-]', 'FontSize', 15)
xlabel('height [m]', 'FontSize', 15)

grid on
end
legend('Ti = 1 %','Ti = 3 %','Ti = 6 %','Location','NorthEast')
