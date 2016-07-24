# This is a simple one Resource / one Consumer model
#
# We essentially work through Chapter 5 of McCann's book "Food Webs"

r2c1 <- newTrophicModel()
r2c1 <- addSpecies(r2c1, "resource1", growthRate = 0.2, carryingCapacity = 25, timeLag = 0)
r2c1 <- addSpecies(r2c1, "resource2", growthRate = 0.2, carryingCapacity = 25, timeLag = 7)
r2c1 <- addSpecies(r2c1, "consumer", mortality = 0.2, halfSaturation = 1)

r2c1 <- addInteraction(r2c1,
                       predator = "consumer",
                       prey = "resource1",
                       attackRate = 0.2,
                       conversionRate = 0.35,
                       timeLag = 0)
r2c1 <- addInteraction(r2c1,
                       predator = "consumer",
                       prey = "resource2",
                       attackRate = 0.2,
                       conversionRate = 0.35,
                       timeLag = 0)
print(r2c1)
initialValues <- newInitialValues(r2c1, c(11, 10, 10))
print(initialValues)
results <- integrateModel(r2c1, 0.1, 10, 1000, initialValues)