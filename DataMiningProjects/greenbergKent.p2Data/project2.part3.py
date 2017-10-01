#Includes
import numpy as np
from sklearn.preprocessing import Imputer
from sklearn.model_selection import cross_val_score
from sklearn import tree
from sklearn.ensemble import RandomForestClassifier
import scipy.stats as stats
import numpy as np
import math
from sklearn.ensemble import RandomForestClassifier
from sklearn.neighbors import KNeighborsClassifier
from sklearn.neural_network import MLPClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import confusion_matrix
from sklearn.metrics import accuracy_score
from sklearn.preprocessing import OneHotEncoder
from sklearn import preprocessing

#reads in data from file
data = np.loadtxt("p2dataPart3.csv", delimiter = ",")
#Get the imputer. Missing values are stored as -1111. Use medians to fill.
imp = Imputer(missing_values= -1111.1, strategy='median',axis=0)
#Separate X from target attribute y
X = data[:,0:4]
y = data[:,6]
#Apply the imputer
X = imp.fit_transform(X)
#Separate training and testing split
Xtrain, Xtest, ytrain, ytest = train_test_split(X, y,
                                                test_size=.2,    # 20%
                                                random_state=0)  # seed)

#Implement different modelling strategies 
Classifier = MLPClassifier(activation='logistic', max_iter = 200,random_state=0)
model = Classifier.fit(X,y)
scores = cross_val_score(model,X,y,cv= 5)
print("\nNeural Net with maxiter = 100, logistic activation...\n")
print(scores)

Classifier = MLPClassifier(activation='relu', max_iter = 200,random_state=0)
model = Classifier.fit(X,y)
scores = cross_val_score(model,X,y,cv= 5)
print("\nNeural Net with maxiter = 100, relu activation... \n")
print(scores)

Classifier = MLPClassifier(learning_rate='adaptive')
model = Classifier.fit(X,y)
scores = cross_val_score(model,X,y,cv= 5)
print("\nNeural Net with adaptive learning rate \n")
print(scores)

Classifier = MLPClassifier(learning_rate='adaptive')
model = Classifier.fit(X,y)
scores = cross_val_score(model,X,y,cv= 5)
print("\nNeural Net with adaptive learning rate \n")
print(scores)

Classifier = MLPClassifier(early_stopping = true, max_iter = 1000)
model = Classifier.fit(X,y)
scores = cross_val_score(model,X,y,cv= 5)
print("\nNeural Net with maxiter = 1000, stops early when val_score stops improving)
print(scores)


Classifier = RandomForestClassifier(n_estimators=5,random_state=0,criterion='entropy') 
model = Classifier.fit(X,y)
scores = cross_val_score(model,X,y,cv= 5)
print("\nRandom Forest with numTrees = 5, default paramaters...\n")
print(scores)

Classifier = RandomForestClassifier(n_estimators=50,random_state=0,criterion='entropy') 
model = Classifier.fit(X,y)
scores = cross_val_score(model,X,y,cv= 5)
print("\nRandom Forest with numTrees = 50, default paramaters...\n")
print(scores)

Classifier = RandomForestClassifier(n_estimators=100,random_state=0,criterion='entropy') 
model = Classifier.fit(X,y)
scores = cross_val_score(model,X,y,cv= 5)
print("\nRandom Forest with numTrees = 100, default paramaters...\n")
print(scores)

Classifier = tree.DecisionTreeClassifier()
model = Classifier.fit(X,y)
scores = cross_val_score(model,X,y,cv= 5)
print("\nSingle Decision Tree, default paramaters...\n")
print(scores)

Classifier = RandomForestClassifier(n_estimators=50,random_state=0,criterion='gini') 
model = Classifier.fit(X,y)
scores = cross_val_score(model,X,y,cv= 5)
print("\nRandom Forest with numTrees = 50, criterion = gini...\n")
print(scores)
