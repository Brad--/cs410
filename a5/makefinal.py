from math import sqrt

fo = open("final.txt", "w")

fo.write("eye 0 15 0\n")
fo.write("up 0 1 0\n")
fo.write("look 0 0 0\n")
fo.write("d 2\n")
fo.write("bounds -1 -1 1 1\n")
fo.write("res 1000 1000\n")
fo.write("ambient 0.1 0.1 0.1\n")
fo.write("diffuse .25 .25 .25\n")
fo.write("specular 1 1 1\n")
fo.write("depth 1\n")
# fo.write("light -10 -5 0 1 .9 .9 .9\n")
# fo.write("light 10 -5 0 1 .9 .9 .9\n")
fo.write("light 0 15 0 1 .9 .9 .9\n")

radius = 2
width = 1
light = "1 1 1"
# r o y g b i v
colors = ["1 0 0", "1 .49 0", "1 1 0", "0 1 0", "0 0 1", ".33 0 .57", ".65 0 .93"]
xs = [-1, -.5, 0, .5, 1,  .5,  0, -.5]
zs = [ 0,  .5, 1, .5, 0, -.5, -1, -.5]
for i in range(200):
    diff = i / 10
    x = (xs[i % 8] * 4);
    # if x > 0:
    #     x -= diff
    # if x < 0:
    #     x += diff

    y = -i % 5

    # z = (zs[i % 8] * 4);
    z = sqrt(abs(radius - x**2))
    if x**2 < 0:
        z = -z

    # if z > 0:
    #     z -= diff
    # if z < 0:
    #     z += diff

    fo.write("sphere " + str(x) + " " + str(y) + " " + str(z) + " " + str(width) + " " + colors[i % 7] + "\n")
    
    if (i % 2) == 0:
        fo.write("sphere 0 " + str(y) + " 0 1 0.1 0.1 0.1\n")

fo.close();