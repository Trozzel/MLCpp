
nums = []
labels = []

with open("/Users/jorge/Cpp/MachineLearning/YouTube/data/t10k-labels-idx1-ubyte", "rb") as fp:
    for i in range(2):
        nums.append(int.from_bytes(fp.read(4), byteorder="big"))
    for label in fp:
        labels.append(label)

for num in nums:
    print(num)

print(labels)