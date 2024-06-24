# import scipy.io
# import numpy as np
# from sklearn.preprocessing import StandardScaler
# from sklearn.preprocessing import PolynomialFeatures
# from sklearn.linear_model import LinearRegression

# ## 数据准备

# trainData = scipy.io.loadmat('E:\\SJTU-Classes\\csMath\\lab2\\dataset\\task1\\PA_data_train.mat')

# trainInput = trainData['paInput'].flatten()
# trainOutput = trainData['paOutput'].flatten()

# m = 2  # 记忆的深度

# def create_memory_matrix(x, m):
#     n = len(x)
#     X = np.zeros((n - m, m + 1), dtype = complex)
#     for i in range(m, n):
#         X[i - m] = x[i - m:i + 1]
#     return X

# X_train = create_memory_matrix(trainInput, m)
# y_train = trainOutput[m:]
# scaler_X = StandardScaler()
# scaler_y = StandardScaler()

# X_train_real = scaler_X.fit_transform(X_train.real)
# X_train_imag = scaler_X.fit_transform(X_train.imag)
# y_train_real = scaler_y.fit_transform(y_train.real.reshape(-1, 1)).flatten()
# y_train_imag = scaler_y.fit_transform(y_train.imag.reshape(-1, 1)).flatten()


# ## 推测函数
# # 创建多项式特征
# poly = PolynomialFeatures(degree=2)
# X_train_real_poly = poly.fit_transform(X_train_real)
# X_train_imag_poly = poly.fit_transform(X_train_imag)
# # 构建和训练多项式回归模型
# model_real = LinearRegression()
# model_imag = LinearRegression()

# model_real.fit(X_train_real_poly, y_train_real)
# model_imag.fit(X_train_imag_poly, y_train_imag)


# ## 测试部分
# # 预测测试数据的输出
# testData = scipy.io.loadmat('E:\\SJTU-Classes\\csMath\\lab2\\dataset\\task1\\PA_data_test.mat')

# testInput = testData['paInput'].flatten()
# testOutput = testData['paOutput'].flatten()
# X_test = create_memory_matrix(testInput, m)
# y_test = testOutput[m:]
# X_test_real = scaler_X.transform(X_test.real)
# X_test_imag = scaler_X.transform(X_test.imag)
# y_test_real = y_test.real
# y_test_imag = y_test.imag
# X_test_real_poly = poly.transform(X_test_real)
# X_test_imag_poly = poly.transform(X_test_imag)

# y_pred_real = model_real.predict(X_test_real_poly)
# y_pred_imag = model_imag.predict(X_test_imag_poly)

# y_pred = y_pred_real + 1j * y_pred_imag

# # 计算NMSE
# def calculate_nmse(true_values, predicted_values):
#     # 分离实部和虚部
#     I_out = true_values.real
#     Q_out = true_values.imag
#     I_pred = predicted_values.real
#     Q_pred = predicted_values.imag
    
#     # 计算NMSE
#     numerator = np.sum((I_out - I_pred)**2 + (Q_out - Q_pred)**2)
#     denominator = np.sum(I_out**2 + Q_out**2)
    
#     nmse = 10 * np.log10(numerator / denominator)
    
#     for i in range(20, 30):
#         print(true_values[i], " ", predicted_values[i])
#     return nmse

# nmse_value = calculate_nmse(y_test, y_pred)
# print(f'NMSE: {nmse_value:.6f}')

import numpy as np
import scipy.io as sio
from sklearn.preprocessing import StandardScaler

# 加载数据
train_data = sio.loadmat('E:\\SJTU-Classes\\csMath\\lab2\\dataset\\task1\\PA_data_train.mat')
test_data = sio.loadmat('E:\\SJTU-Classes\\csMath\\lab2\\dataset\\task1\\PA_data_test.mat')

x_train = train_data['paInput'].flatten()
y_train = train_data['paOutput'].flatten()
x_test = test_data['paInput'].flatten()
y_test = test_data['paOutput'].flatten()

m = 4  # 记忆深度

def create_memory_matrix(x, m):
    n = len(x)
    X = np.zeros((n - m, m + 1), dtype=complex)
    for i in range(m, n):
        X[i - m] = x[i - m:i + 1]
    return X

X_train = create_memory_matrix(x_train, m)
X_test = create_memory_matrix(x_test, m)
y_train = y_train[m:]
y_test = y_test[m:]

# 分离实部虚部
scaler = StandardScaler()
X_train_real = scaler.fit_transform(X_train.real)
X_train_imag = scaler.fit_transform(X_train.imag)
X_test_real = scaler.transform(X_test.real)
X_test_imag = scaler.transform(X_test.imag)

y_train_real = y_train.real
y_train_imag = y_train.imag
y_test_real = y_test.real
y_test_imag = y_test.imag

import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense

# 创建神经网络
def create_nn_model(input_shape):
    model = Sequential([
        Dense(64, activation='relu', input_shape=(input_shape,)),
        Dense(64, activation='relu'),
        Dense(1)
    ])
    model.compile(optimizer='adam', loss='mse')
    return model

input_shape = X_train_real.shape[1]

model_real = create_nn_model(input_shape)
model_imag = create_nn_model(input_shape)

# 训练模型
model_real.fit(X_train_real, y_train_real, epochs=100, batch_size=32, verbose=0)
model_imag.fit(X_train_imag, y_train_imag, epochs=100, batch_size=32, verbose=0)

# 用测试数据预测
y_pred_real = model_real.predict(X_test_real).flatten()
y_pred_imag = model_imag.predict(X_test_imag).flatten()

y_pred = y_pred_real + 1j * y_pred_imag


# 计算NMSE
def calculate_nmse(true_values, predicted_values):
    # 分离实部和虚部
    I_out = true_values.real
    Q_out = true_values.imag
    I_pred = predicted_values.real
    Q_pred = predicted_values.imag
    
    # 计算NMSE
    numerator = np.sum((I_out - I_pred)**2 + (Q_out - Q_pred)**2)
    denominator = np.sum(I_out**2 + Q_out**2)
    
    nmse = 10 * np.log10(numerator / denominator)
    
    for i in range(10, 100):
        print(true_values[i], " ", predicted_values[i])
    return nmse

nmse_value = calculate_nmse(y_test, y_pred)
print(f'NMSE: {nmse_value:.6f}')
