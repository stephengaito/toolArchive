# This is a simple one Resource / one Consumer model
#
# We essentially work through Chapter 5 of McCann's book "Food Webs"

r1c1 <- newTrophicModel()
r1c1 <- addSpecies(r1c1, "resource", growthRate = 0.2, carryingCapacity = 25)
r1c1 <- addSpecies(r1c1, "consumer", mortality = 0.2, halfSaturation = 40)

r1c1 <- addInteraction(r1c1,
                       predator = "consumer",
                       prey = "resource",
                       attackRate = 0.5,
                       conversionRate = 0.75)
#print(r1c1)
numSpecies <- numSpeciesInModel(r1c1)
initialValues <- matrix( c(100.0, 1.0), nrow = 1, ncol=numSpecies)
#print(initialValues)
results <- integrateModel(r1c1, 0.1, 10, 1000, initialValues)