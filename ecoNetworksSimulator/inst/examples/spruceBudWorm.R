# This is a complex Spruce Budworm model as suggested by the paper
#
#   "Fluctuations in Density of an Outbreak Species Drive Diversity Cascades in Food Webs"
#   by: Eldon S. Eveleigh, Kevin S. McCann, Peter C. McCarthy, Steven .T. Pollock,
#     Christopher .T. Lucarotti, Benoit Morin, George A. McDougall, Douglas B. Strongman, 
#     John T. Huber, James Umbanhowar and Lucas D. B. Faria
#   in: Proceedings of the National Academy of Sciences of the United States of America,
#     Vol. 104, No. 43 (Oct. 23, 2097), pp. 16976-16981
#
# See figure 1.{A,B,C} 
#
species     <- newSpeciesTable()
speciesStds <- newSpeciesTable()
# Trophic level 1 
species    <- addSpecies(species,     "Balsam Fir", growthRate = 0.2, carryingCapacity = 25)
speciesStd <- addSpecies(speciesStds, "Balsam Fir", growthRate = 0.1, carryingCapacity = 5)
# Trophic level 2
species     <- addSpecies(species,     "Coleotechnites atrupictella", mortality = 0.2, halfSaturation = 40)
speciesStds <- addSpecies(speciesStds, "Coleotechnites atrupictella", mortality = 0.1, halfSaturation = 5)
species     <- addSpecies(species, "Coleotechnites picaella", mortality = 0.2, halfSaturation = 40)
speciesStds <- addSpecies(species, "Coleotechnites picaella", mortality = 0.2, halfSaturation = 40)
species <- addSpecies(species, "Acleris variana", mortality = 0.2, halfSaturation = 40)
species <- addSpecies(species, "Choristoneura fumiferana", mortality = 0.2, halfSaturation = 40)
species <- addSpecies(species, "Epinota radicana", mortality = 0.2, halfSaturation = 40)
species <- addSpecies(species, "Choristoneura rosaceana", mortality = 0.2, halfSaturation = 40)

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