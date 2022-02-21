# This is a complex Spruce Budworm model as suggested by the paper
#
#   "Fluctuations in Density of an Outbreak Species Drive Diversity Cascades in Food Webs"
#   by: Eldon S. Eveleigh, Kevin S. McCann, Peter C. McCarthy, Steven J. Pollock,
#     Christopher J. Lucarotti, Benoit Morin, George A. McDougall, Douglas B. Strongman, 
#     John T. Huber, James Umbanhowar and Lucas D. B. Faria
#   in: Proceedings of the National Academy of Sciences of the United States of America,
#     Vol. 104, No. 43 (Oct. 23, 2007), pp. 16976-16981
#
# See figure 1.{A,B,C} 
#
# The species code is listed in the "spruceBudWormSpecies.csv" file

rndModel <- newTrophicModel()
rndModel <- loadSpecies(rndModel,      "spruceBudWormSpecies.csv")
rndModel <- loadInteractions(rndModel, "spruceBudWormInteractions.csv")
print(rndModel)

aModel <- varyModel(rndModel)
print(aModel)

initialValues <- createInitialValuesFor(aModel, c(100.0, 1.0))
print(initialValues)

results <- integrateModel(aModel, 0.1, 10, 1000, initialValues)