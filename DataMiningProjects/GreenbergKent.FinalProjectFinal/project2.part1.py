# Brian Greenberg and Nate Kent
# 4/28/2017
# Project 2 - Part 1
# Run this file from the command line with python to evaluate our datasets
# This part takes data from p2data.csv and imputes via median strategy
# Part 2 avoids imputing values and deletes all rows with missing values
# Part 3 also deletes rows and includes a select group of attributes that 
# we believed to be the most influential. We found that the imputing led 
# to the addition of noise, and ultimately less predictive accuracy.
# Part 2 and 3 perform slightly better than part 1.
# This will evaluate our dataset by a variety of classification models.
# It will also generate confusion matrices, p-values, and confidence intervals.


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

#Function for generating the confusion matrix for a model
def printConfusionMatrix(model,X,Y):
    Xtrain, Xtest, ytrain, ytest = train_test_split(X, y,
                                                test_size=.2,
                                                random_state=10)  
    # use the model on the training data
    # (whatever your model variable was last set to)
    model.fit(Xtrain, ytrain)
    y_model = model.predict(Xtest)
    score1 = accuracy_score(ytest, y_model)
    mat = confusion_matrix(ytest, y_model)
    print(mat)

#Function for getting the average of samples
def sample_mean(samples):
  resultList = []
  for i in range(0,len(samples)):
    curr = samples[i]
    mean = curr.mean()
    resultList.append(mean)
  return resultList

#Function for calculating confidence of samples
def calculate_confidence(samples):
  z_critical = stats.norm.ppf(q = 0.975)
  resultList = []

  for i in range(0,len(samples)):
    curr = samples[i]
    mean = curr.mean()
    stdDev = curr.std()
    error = z_critical * stdDev
    result = []
    result.append(mean - error)
    result.append(mean + error)
    resultList.append(result)
  return resultList  

#Function for displaying confidence intervals on a plot
def displayConfidenceIntervals(samples,name,labels):
    sample_means = sample_mean(samples)
    intervals = calculate_confidence(samples)
    #print(intervals)
    import matplotlib.pyplot as plt
    plt.figure(figsize=(10,10))

    xvals = np.arange(5, 30, 5)
    yerrors = [(top-bot) / 2 for top,bot in intervals]

    plt.errorbar(x=xvals,
             y=sample_means,
             yerr=yerrors,
             fmt='D')
    plt.axis(xmin=0,xmax=30)
    plt.xticks(xvals,labels)

    plt.savefig(name, bbox_inches='tight')
    plt.show()

#reads in data from file
data = np.loadtxt("p2data.csv", delimiter = ",")
#Get the imputer. Missing values are stored as -1111. Use medians to fill.
imp = Imputer(missing_values= -1111.1, strategy='median',axis=0)

#Separate X from target attribute y
X = data[:,0:20]
y = data[:,22]
#Apply the imputer
X = imp.fit_transform(X)
#Separate training and testing split
Xtrain, Xtest, ytrain, ytest = train_test_split(X, y,
                                                test_size=.2,    # 20%
                                                random_state=0)  # seed)

#Implement different modelling strategies 
Classifier = MLPClassifier(activation='logistic', max_iter = 200,random_state=0)
model = Classifier.fit(X,y)
scores1 = cross_val_score(model,X,y,cv= 5)
print("\nNeural Net with maxiter = 100, logistic activation...")
print(scores1)
printConfusionMatrix(model, X,y)

Classifier = MLPClassifier(activation='relu', max_iter = 200,random_state=0)
model = Classifier.fit(X,y)
scores2 = cross_val_score(model,X,y,cv= 5)
print("\nNeural Net with maxiter = 100, relu activation...")
print(scores2)

Classifier = MLPClassifier(learning_rate='adaptive')
model = Classifier.fit(X,y)
scores3 = cross_val_score(model,X,y,cv= 5)
print("\nNeural Net with adaptive learning rate")
print(scores3)

Classifier = MLPClassifier(learning_rate='invscaling')
model = Classifier.fit(X,y)
scores4 = cross_val_score(model,X,y,cv= 5)
print("\nNeural Net with inverse scaling learning rate")
print(scores4)

Classifier = MLPClassifier(early_stopping = True, max_iter = 1000)
model = Classifier.fit(X,y)
scores5 = cross_val_score(model,X,y,cv= 5)
print("\nNeural Net with maxiter = 1000, stops early when val_score stops improving")
print(scores5)

MLPSamples = [scores1,scores2,scores3,scores4,scores5]
p = .05
text = ""
print("Testing for significant differences among MLP methods...")
print("0 = Neural Net with maxiter = 100, logistic activation...")
print("1 = Neural Net with maxiter = 100, relu activation....")
print("2 = Neural Net with adaptive learning rate")
print("3 = Neural Net with inverse scaling learning rate")
print("4 = Neural Net with maxiter = 1000, stops early when val_score stops improving")
for i in range(0,5):
  for j in range(0,5):
    overP = ""
    if i != j:
      if j > i:
        pair01 = stats.ttest_rel(MLPSamples[i],MLPSamples[j])
        if pair01[1] > p:
          overP = " Value exceeds P value of " + str(p)
        text += "The p-value for (" + str(i) + "," +  str(j) + ") is " 
        text += str(pair01[1]) + str(overP) + "\n"
print(text)
labels = ['0','1','2','3','4']
displayConfidenceIntervals(MLPSamples, "MLPConfidenceIntervals1.png", labels)

Classifier = RandomForestClassifier(n_estimators=5,random_state=0,criterion='entropy') 
model = Classifier.fit(X,y)
scores1 = cross_val_score(model,X,y,cv= 5)
print("\nRandom Forest with numTrees = 5, default paramaters...")
print(scores1)
printConfusionMatrix(model, X,y)

Classifier = RandomForestClassifier(n_estimators=50,random_state=0,criterion='entropy') 
model = Classifier.fit(X,y)
scores2 = cross_val_score(model,X,y,cv= 5)
print("\nRandom Forest with numTrees = 50, default paramaters...")
print(scores2)

Classifier = RandomForestClassifier(n_estimators=100,random_state=0,criterion='entropy') 
model = Classifier.fit(X,y)
scores3 = cross_val_score(model,X,y,cv= 5)
print("\nRandom Forest with numTrees = 100, default paramaters...")
print(scores3)

Classifier = tree.DecisionTreeClassifier()
model = Classifier.fit(X,y)
scores4 = cross_val_score(model,X,y,cv= 5)
print("\nSingle Decision Tree, default paramaters...")
print(scores4)

Classifier = RandomForestClassifier(n_estimators=50,random_state=0,criterion='gini') 
model = Classifier.fit(X,y)
scores5 = cross_val_score(model,X,y,cv= 5)
print("\nRandom Forest with numTrees = 50, criterion = gini...")
print(scores5)

RFSamples = [scores1,scores2,scores3,scores4,scores5]
p = .05
text = ""

print("Testing for significant differences among RF/tree methods...")
print("0 = Random Forest with numTrees = 5, default paramaters...")
print("1 = Random Forest with numTrees = 50, default paramaters...")
print("2 = Random Forest with numTrees = 100, default paramaters...")
print("3 = Single Decision Tree, default paramaters...")
print("4 = Random Forest with numTrees = 50, criterion = gini...")

for i in range(0,5):
  for j in range(0,5):
    overP = ""
    if i != j:
      if j > i:
        pair01 = stats.ttest_rel(RFSamples[i],RFSamples[j])
        if pair01[1] > p:
          overP = " Value exceeds P value of " + str(p)
        text += "The p-value for (" + str(i) + "," +  str(j) + ") is " 
        text += str(pair01[1]) + str(overP) + "\n"
print(text)

labels = ['0','1','2','3','4']
displayConfidenceIntervals(RFSamples, "RFConfidenceIntervals1.png", labels)
