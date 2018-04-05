%% triangle creater
xmin = 0;
xmax = 4;
ymin = 0;
ymax = 4;

Npoints = 10;
ystep = ymax/Npoints;
xstep = xmax/Npoints;

% y(1) = 0;
% x(1) = 4;
% i = 1;
y_count = 0;
x_count = 4;

%straight line
while i < Npoints
    x(i) = xmax;
    y_count = y_count + ystep;
    y(i) = y_count;
    i=i+1;
end

% i = 1;
%going down till y = 2
while i < 1.5*Npoints
    x_count = x_count-xstep;
    x(i) = x_count;
    y_count = y_count - ystep;
    y(i) = y_count;
    i= i+1;
end

% goint back to initial point
while i < 2*Npoints
    x_count = x_count+xstep;
    x(i) = x_count;
    y_count = y_count - ystep;
    y(i) = y_count;
    i= i+1;
end

%% making a nice plot
figure(1)
hold on 
plot(x,y)