import numpy as np
import cv2
import matplotlib.pyplot as plt
import cvxpy as cp

# 创建需要被拟合的图像
def create_image(size, radius):
    image = np.ones((size, size)) * 0.99
    center = size // 2
    Y, X = np.ogrid[:size, :size]
    dist_from_center = np.sqrt((X - center)**2 + (Y - center)**2)
    mask = dist_from_center <= radius
    image[mask] = 0.01
    return image

# 创建需要被拟合的另一个图像
def create_image_triangle(size, base):
    image = np.ones((size, size)) * 0.99
    for i in range(size):
        for j in range(size):
            if i > size / 2  - base / 4 and i < size / 2 + base / 4 and \
               j > size / 2 - i + base / 2 and j < size / 2 + i - base / 2:
                image[i,j] = 0.01
    return image

# 一个会出现分离异常的例子
def create_image_triangle_abnormal(size, base):
    image = np.ones((size, size)) * 0.99
    for i in range(size):
        for j in range(size):
            if i > size / 2  - base / 4 and i < size / 2 + base / 4 and \
               j > size / 2 - i + base / 2 and j < size / 2 + i - base / 2:
                image[i,j] = 0.01 * (i / 2 + j / 3)
    return image

def solveProblem(image, name):
    # 打印原始图像
    plt.title(name + " Original Picture")
    plt.imshow(image, cmap='gray')
    plt.show()
#    plt.savefig(name + "Origial Picture.png")
    # cv2.imshow('Origin circle', image)
    # cv2.waitKey(0)
    # cv2.destroyAllWindows()

    # 设置变量、常量、优化目标，并求解
    ones = np.ones((size, size))
    onesConstant = cp.Constant(ones)
    imgConstant = cp.Constant(image)
    x = cp.Variable((size, size), boolean = True)
    objective = cp.Minimize(cp.sum(cp.multiply(x, cp.square(imgConstant))) + cp.sum(cp.multiply(onesConstant - x, cp.square(onesConstant - imgConstant))))
    problem = cp.Problem(objective)
    problem.solve(solver = cp.ECOS_BB)

    # 打印结果图像
    plt.title(name + "Picture formed from x")
    plt.imshow(x.value, cmap='gray')
    plt.show()
#    plt.savefig(name + "formed Picture.png")
    # cv2.imshow('Result Image:', x.value)
    # cv2.waitKey(0)
    # cv2.destroyAllWindows()

    x = ones - x
    plt.title(name + "Inversed from x")
    plt.imshow(x.value, cmap='gray')
    plt.show()
#    plt.savefig(name + "Analized Picture.png")


size = 100 # Image size
radius = 30 # Circle radius
base = 80 # Triangle base length

image = create_image(size, radius)
solveProblem(image, "circle")

image = create_image_triangle(size, base)
solveProblem(image, "triangle")
