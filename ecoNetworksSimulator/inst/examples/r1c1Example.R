# This is a simple one Resource / one Consumer model
#
# We essentially work through Chapter 5 of McCann's book "Food Webs"

species <- newSpeciesTable()
species <- addSpecies(species, "resource", growthRate = 0.2, carryingCapacity = 25)
species <- addSpecies(species, "consumer", mortality = 0.2, halfSaturation = 40)
interactions <- newInteractionsTable()
interactions <- addInteraction(interactions,
                               predator = "consumer",
                               prey = "resource",
                               attackRate = 0.5,
                               conversionRate = 0.75)
r1c1 <- newModel(species, interactions)
#print(r1c1)
numSpecies <- numSpeciesInModel(r1c1)
initialValues <- matrix( c(100.0, 1.0), nrow = 1, ncol=numSpecies)
#print(initialValues)
results <- integrateModel(r1c1, 0.1, 10, 1000, initialValues)