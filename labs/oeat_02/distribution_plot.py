import sys
import matplotlib.pyplot as plt

x = []
y = []

frequency_file = sys.argv[1]
with open(frequency_file) as file:
  for line in file.readlines():
    y.append(int(line.split()[0]))
    x.append(len(y))

yPercent = [round(value / y[0] * 100) for value in y]
yZiffian = [100 / value for value in x]

plt.plot(x, yPercent, label = "Result")
plt.plot(x, yZiffian, label = "Ziffian Curve", ls = ':', color = "red")

plt.xlabel("rank")
plt.ylabel("frequency")
plt.legend()
plt.show()
