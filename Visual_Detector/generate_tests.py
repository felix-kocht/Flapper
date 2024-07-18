params = [0.4,38,20,90,0,0,0,10]

for i in range(5):
    for j in range(10):
        print(f'{params[0]:.1f},{params[1]},{params[2]},{params[3]},{params[4]},{params[5]},{params[6]},{params[7]}')
        params[2] += 5
    params[2] = 20
    params[0] += 0.10000000000000000