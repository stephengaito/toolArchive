## ----echo=FALSE----------------------------------------------------------
library(rgl)
library(rglwidget)
library(dynSysToolKit)
rgl.open()
rgl.bg(color="white")
.setupKnitr()

## ------------------------------------------------------------------------
results <- integrateLorenz()
rgl.linestrips(results[,1], results[,2], results[,3], color = "blue")
subid <- currentSubscene3d()
rglwidget(elementId = "lorenzEq")

