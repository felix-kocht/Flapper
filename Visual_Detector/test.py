params = [0.5, 40,15,90,0,0,0,10]

for i in range(4):
    for j in range(13):
        print(f'{params[0]:.1f},{params[1]},{params[2]},{params[3]},{params[4]},{params[5]},{params[6]}, {params[7]}')
        params[2] += 5
    params[2] = 15
    params[0] += 0.10000000000000000