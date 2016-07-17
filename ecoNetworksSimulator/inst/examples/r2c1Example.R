# This is a simple one Resource / one Consumer model
#
# We essentially work through Chapter 5 of McCann's book "Food Webs"

species <- newSpeciesTable()
species <- addSpecies(species,
  "resource1", growthRate = 0.2, carryingCapacity = 25, timeLag = 0)
species <- addSpecies(species,
  "resource2", growthRate = 0.2, carryingCapacity = 25, timeLag = 7)
species <- addSpecies(species,
  "consumer", mortality = 0.2, halfSaturation = 1)
interactions <- newInteractionsTable()
interactions <- addInteraction(interactions,
                               predator = "consumer",
                               prey = "resource1",
                               attackRate = 0.2,
                               conversionRate = 0.35,
                               timeLag = 0)
interactions <- addInteraction(interactions,
                               predator = "consumer",
                               prey = "resource2",
                               attackRate = 0.2,
                               conversionRate = 0.35,
                               timeLag = 0)
r2c1 <- newModel(species, interactions)
print(r2c1)
numSpecies <- numSpeciesInModel(r2c1)
initialValues <- matrix( c(11, 10, 10), nrow = 1, ncol=numSpecies)
print(initialValues)
results <- integrateModel(r2c1, 0.1, 10, 1000, initialValues)