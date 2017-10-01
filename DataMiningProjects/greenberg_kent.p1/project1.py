"""
Lab 9, CSCI 2600
Brian Greenberg & Nathaniel Kent
March 14, 2017
"""


#This file contains the script to compare and illustrate our learning models.
#Simply compile this file with no arguments and it will run on the UCIWine dataset.
#The Dataset is defined outside of the main function
#This code is compatable with other datasets from url (Just change the url name).
#We are assuming five-fold cross validation, this is also declared as a global var outside of main

import urllib.request          # to retrieve URL files
from sklearn.model_selection import cross_val_score
from sklearn import tree
import scipy.stats as stats
import numpy as np
import math
from sklearn.ensemble import RandomForestClassifier
from sklearn.neighbors import KNeighborsClassifier
from sklearn.neural_network import MLPClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import confusion_matrix
from sklearn.metrics import accuracy_score

#Function for generating the confusion matrix for a model
def printConfusionMatrix(model,X,Y,name):
    Xtrain, Xtest, ytrain, ytest = train_test_split(X, y,
                                                test_size=.2,
                                                random_state=10)  
    # use the model on the training data
    # (whatever your model variable was last set to)
    model.fit(Xtrain, ytrain)
    y_model = model.predict(Xtest)
    score1 = accuracy_score(ytest, y_model)
    mat = confusion_matrix(ytest, y_model)
    f = open(name, "w+")
    f.write( str(mat))
    f.close


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
def displayConfidenceIntervals(samples, name, labels):
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

##############################################
#Functions for the different classification approaches
###############################################


#Neural nets
#Variable layer layersizes and maximum iterations
def makeNeuralNet(X,y,layerSize, maxIter):
    print("Modeling Neural Nets with layersize:")
    print(layerSize)
    print("and maxiterations: ")
    print(maxIter)
    ## MLP: multi-layer perceptron
    from sklearn.neural_network import MLPClassifier

    # Initialize ANN classifier
    # Note: all of these parameters have default values
    #   and could therefore be optional
    model = MLPClassifier(hidden_layer_sizes=(layerSize,layerSize),
 # a tuple; default is (100,)
                      activation='logistic',      # default is 'relu'
                      max_iter = maxIter,             # default is 200
                      random_state=0)             # seed

    # one t-t split (not a cross-validation)
    from sklearn.model_selection import train_test_split
    Xtrain, Xtest, ytrain, ytest = train_test_split(X, y,
                                                test_size=.2,    # 20%
                                                    random_state=0)  # seed
    # for NN's, need to have all values on [0..1] range; 
    #   StandardScaler does this for us 
    from sklearn.preprocessing import StandardScaler
    scaler = StandardScaler()
    # Fit only to the training data
    scaler.fit(Xtrain)

    # Now apply the transformations to the data:
    Xtrain = scaler.transform(Xtrain)
    Xtest = scaler.transform(Xtest)

    # will need to scale all of X, for cross-val
    X = scaler.transform(X)

    # Train the classifier with the training data
    model.fit(Xtrain,ytrain)
    return model

#Decision Tree
#Variable decision tree max depth
def makeDecisionTree(X,y,depth):
    print("Modeling the Decision tree with a depth of " + str(depth))
    dt = tree.DecisionTreeClassifier(criterion='entropy', max_depth=depth)
    model = dt.fit(X,y)
    return model

#Random Forest
#Variable number of trees
def makeRandomForest(X,y, numTrees):
    print("Modeling the Random Forest with a numTrees = " + str(numTrees)) 
    rf2 = RandomForestClassifier(n_estimators=numTrees,         #make 100 trees
                               random_state=0,        #seed to random # gen
                                 criterion='entropy')   #use entropy
    forest_model = rf2.fit(X, y)
    return forest_model

#Nearest Neighbors
#Variable K-value
def makeKNNModel(X, y, k):
    print("Modeling the KNN with k nearest neighbors = " + str(k))
    model = KNeighborsClassifier(n_neighbors=k,
                                     weights='uniform')  #default
    # one t-t split (not a cross-validation)
    Xtrain, Xtest, ytrain, ytest = train_test_split(X, y,
                                                test_size=.2,    # 20%
                                                        random_state=0)  # seed
    # train knn
    model.fit(Xtrain, ytrain)
    # predictions 
    return model

############################################################

#Generate p-values for a set of samples and write significant differences to file
def generatePValues(samples,p):
    #Determine which differences are significant and write to file...
    text = ""
    text += "Testing for significance in samples: \n"
    for i in range(0,5):
        for j in range(0,5):
            overP = ""
            if i != j:
                if j > i:
                    pair01 = stats.ttest_rel(samples[i],samples[j])
                    if pair01[1] > p:
                        overP = "Value exceeds P value of " + str(p)
                    text += "The p-value for (" + str(i) + "," +  str(j)
                    text += ") is " + str(pair01[1]) + str(overP) + "\n"

    #Write out text string to file...
    f = open("p-values.txt", "w+")
    f.write(text)
    f.close

################################################################
#Load UCI wine dataset

#Url
url = "http://www.bowdoin.edu/~congdon/Courses"
url += "/2600/Data/Wine/winequality-white.csv"

#Get raw data from url fit parse X and y values as floats
raw_data = urllib.request.urlopen(url)
X = np.loadtxt(raw_data, delimiter=";", usecols= range(11), dtype='float64')
raw_data = urllib.request.urlopen(url)
y = np.loadtxt(raw_data, delimiter=";", usecols= [11], dtype='float64')

print("X shape is", X.shape)
print("y shape is", y.shape)

crossVals = 5     #sets the crossValidation to 5

################################################################
def main():

  #loop through and run are different experiments
  treeSamples = []
  for i in range(1,6):
    model = makeDecisionTree(X,y, i * 2)
    scores = cross_val_score(model, X,y, cv= crossVals)
    treeSamples.append(scores)
  forestSamples = []
  for i in range(1,6):
    model = makeRandomForest(X,y, 20 * i)
    scores = cross_val_score(model, X,y, cv= crossVals)
    forestSamples.append(scores)
  knnSamples = []
  for i in range(1,6):
    model = makeKNNModel(X,y,i)
    scores = cross_val_score(model, X,y, cv= crossVals)
    knnSamples.append(scores)
  annSamples = []
  for i in range(1,6):
    model = makeNeuralNet(X,y,i*10,2000)
    scores = cross_val_score(model, X,y, cv = crossVals)
    annSamples.append(scores)

  #Confusion matrix for the last model
  printConfusionMatrix(model,X,y,"matrix.txt")

  #Display some confidence intervals
  labels = ["d=2", "d=4", "d=6", "d=8", "d=10"]
  displayConfidenceIntervals(treeSamples, "ConfidenceDT", labels)
  labels = ["e=20", "e=40", "e=60", "e=80", "e=100"]
  displayConfidenceIntervals(forestSamples, "ConfidenceRF", labels)
  labels = ["k=1", "k=2", "k=3", "k=4", "k=5"]
  displayConfidenceIntervals(knnSamples, "ConfidenceKNN", labels)
  labels = ["l=10", "l=20", "l=30", "l=40", "l=50"]
  displayConfidenceIntervals(annSamples, "ConfidenceANN", labels)
  
main()
