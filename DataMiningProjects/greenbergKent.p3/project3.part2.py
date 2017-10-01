import matplotlib.pyplot as plt      # we'll need this to plot
from sklearn import datasets
from sklearn.datasets import load_iris

iris = datasets.load_iris()
X = iris.data # we only take the first two features.
y = iris.target

print("X shape is", X.shape)
print("y shape is", y.shape)

from sklearn import decomposition
pca = decomposition.PCA(n_components=2) # project from 64 to 2 dimensions

# get the transformed X and print the shape
Xprojected = pca.fit_transform(X)
print("Xprojected.shape is", Xprojected.shape)

# plot it
#   here, we are plotting the attribute 0 on the X axis, 
#     and attribute 1 on the y axis
#   The coloring follows the actual classes, y (c=y)
plt.scatter(Xprojected[:, 0], Xprojected[:, 1],    # x and y axis of plot
            c=y,                                   # set the color
            cmap='viridis')                        # color scheme
plt.xlabel('component 1')
plt.ylabel('component 2')

#plt.show()


from sklearn.cluster import KMeans
kmeans = KMeans(n_clusters=10,     # 10 clusters
                n_init=1,           # but only repeat one time (one search)
                init='random'       # we'll start at random, not "smart"
               )
kmeans.fit(X)
y_kmeans = kmeans.predict(X)
kmeans.fit(Xprojected)                  # fit the projected data (2d)
y_kmeans = kmeans.predict(Xprojected)   # get the cluster ID

# plot the points, with colors
plt.scatter(Xprojected[:, 0], Xprojected[:, 1], 
            c=y_kmeans,                 # cluster ID, not actual class
            cmap='viridis')
plt.xlabel('component 1')
plt.ylabel('component 2')
plt.show()

# Four axes, returned as a 2-d array
f, axarr = plt.subplots(2, 3)
axarr[0, 0].scatter(X[:, 0], X[:, 1], c=y_kmeans, s=10, cmap='viridis')
axarr[0, 0].set_title('attributes [0,1]')
axarr[0, 1].scatter(X[:, 0], X[:, 2], c=y_kmeans, s=10, cmap='viridis')
axarr[0, 1].set_title('attributes [0,2]')
axarr[0, 2].scatter(X[:, 0], X[:, 3], c=y_kmeans, s=10, cmap='viridis')
axarr[0, 2].set_title('attributes [0,3]')

axarr[1, 0].scatter(X[:, 1], X[:, 2], c=y_kmeans, s=10, cmap='viridis')
axarr[1, 0].set_title('attributes [1,2]')
axarr[1, 1].scatter(X[:, 1], X[:, 3], c=y_kmeans, s=10, cmap='viridis')
axarr[1, 1].set_title('attributes [1,3]')
axarr[1, 2].scatter(X[:, 2], X[:, 3], c=y_kmeans, s=10, cmap='viridis')
axarr[1, 2].set_title('attributes [2,3]')

plt.suptitle("pairwise attributes illustration")

plt.show()
