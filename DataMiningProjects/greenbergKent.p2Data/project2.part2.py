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
data = np.loadtxt("p2dataPart2.csv", delimiter = ",")
imp = Imputer(missing_values= -1111.1, strategy='median',axis=0)
data = data.astype(int)
X = data[:,0:20]
y = data[:,22]
X = imp.fit_transform(X)
Xtrain, Xtest, ytrain, ytest = train_test_split(X, y,
                                                test_size=.2,    # 20%
                                                random_state=0)  # seed)

Classifier = MLPClassifier(activation='logistic', max_iter = 100,random_state=0)
model = Classifier.fit(X,y)
scores = cross_val_score(model,X,y,cv= 5)
print("\nNeural Net with maxiter = 100, logistic activation...\n")
print(scores)

Classifier = MLPClassifier(activation='relu', max_iter = 100,random_state=0)
model = Classifier.fit(X,y)
scores = cross_val_score(model,X,y,cv= 5)
print("\nNeural Net with maxiter = 100, relu activation... \n")
print(scores)

Classifier = MLPClassifier(activation='logistic', max_iter = 100, hidden_layer_sizes = (10,10,10))
model = Classifier.fit(X,y)
scores = cross_val_score(model,X,y,cv= 5)
print("\nNeural Net with maxiter = 100, layersizes = 10,10,10...\n")
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
