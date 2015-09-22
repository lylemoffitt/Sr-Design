d = 18;                 
x = [-14*ones(1,7) -13 -12 -11 -10*ones(1,7) -8*ones(1,7) -7*ones(1,3) ...
     -6*ones(1,3) -5*ones(1,2) -4*ones(1,2) -2*ones(1,7) -1 0 1 2 ...
     4*ones(1,7) 5 6 7 8 10*ones(1,5) 11*ones(1,2) 12*ones(1,2) ...
     13*ones(1,2) 14*ones(1,5) -14*ones(1,6) -13 -12*ones(1,2) -11 ...
     -10*ones(1,6) -8*ones(1,5) -7*ones(1,2) -6*ones(1,2) -5*ones(1,2) ...
     -4*ones(1,5) -2*ones(1,7) -ones(1,2) zeros(1,2) ones(1,2) ...
     2*ones(1,5) 4*ones(1,7) 5 6 7 8 10*ones(1,7) 11*ones(1,2) ...
     12*ones(1,2) 13*ones(1,2) 14*ones(1,5)];
z = [3 4 5 6 7 8 9 6 6 6 3 4 5 6 7 8 9 3 4 5 6 7 8 9 3 6 9 3 6 9 ...
     3 9 3 9 3 4 5 6 7 8 9 3 3 3 3 3 4 5 6 7 8 9 3 3 3 3 4 5 6 7 ... 
     8 3 9 3 9 3 9 4 5 6 7 8 -3 -4 -5 -6 -7 -8 -9 -7 -8 -9 -3 -4 ...
     -5 -6 -7 -8 -4 -5 -6 -7 -8 -3 -9 -3 -9 -3 -9 -4 -5 -6 -7 -8 ...
     -3 -4 -5 -6 -7 -8 -9 -3 -6 -3 -6 -3 -6 -4 -5 -7 -8 -9 -3 -4 ...
     -5 -6 -7 -8 -9 -9 -9 -9 -9 -3 -4 -5 -6 -7 -8 -9 -3 -9 -3 -9 ...
     -3 -9 -4 -5 -6 -7 -8];             

scatter(x,z)

[Theta,Rho] = cart2pol(x,z);
Theta_bits = 256;
Theta_allowed = 0:(2*pi/Theta_bits):2*pi*(1-1/Theta_bits);
Rho_approx = round(Rho);
index = 0;
difference = (Theta_allowed(2)-Theta_allowed(1))/2;
Theta_approx = zeros(1,numel(Theta));
Theta_new = zeros(1,numel(Theta));

for i = 1:numel(Theta)
    if(Theta(i) < 0)
        Theta_new(i) = Theta(i) + (2*pi);
    else
        Theta_new(i) = Theta(i);
    end;
    for j = 1:numel(Theta_allowed) 
        if Theta_new(i) > Theta_allowed(j)
            index = j;
        end;
    end;
  if(index ~= 1)
    Theta_testLow = Theta_new(i)-difference-Theta_allowed(index-1);
    Theta_testHigh = Theta_allowed(index)-Theta_new(i)+difference;
    if(Theta_testLow < Theta_testHigh)
        Theta_approx(i) = Theta_allowed(index-1);
    else
        Theta_approx(i) = Theta_allowed(index);
    end;
  else
    Theta_approx(i) = Theta_allowed(index);
  end;
end;

R = Rho_approx - 3;
T = Theta_approx*Theta_bits/(2*pi);

n = length(T);
for j = 2:n
    pivot = T(j);
    pivot1 = R(j);
    i = j;
    while ((i > 1) && (T(i - 1) > pivot))
        T(i) = T(i - 1);
        R(i) = R(i - 1);
        i = i - 1;
    end;
    T(i) = pivot;
    R(i) = pivot1;
end;
figure();

polar(T*(2*pi)/Theta_bits, R+3, '.');