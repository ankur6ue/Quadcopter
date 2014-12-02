...function DrawCube ( origin, sz )
origin  = [0 0 0];
sz    = [2 2 2];

x=([0 1 1 0 0 0;1 1 0 0 1 1;1 1 0 0 1 1;0 1 1 0 0 0]-0.5)*sz(1)+origin(1);
y=([0 0 1 1 0 0;0 1 1 0 0 0;0 1 1 0 1 1;0 0 1 1 1 1]-0.5)*sz(2)+origin(2);
z=([0 0 0 0 0 1;0 0 0 0 0 1;1 1 1 1 0 1;1 1 1 1 0 1]-0.5)*sz(3)+origin(3);
% for i=1:6
%     h=patch(x(:,i),y(:,i),z(:,i),'r');
%     set(h,'edgecolor','b')
% end
c = reshape([[1 0 0]; [0 1 0]; [0 0 1]; [1 0 0]; [0 1 0]; [0 0 1]], 1, 6, 3);
h=patch(x,y,z,c);
% set(h,'edgecolor','b')