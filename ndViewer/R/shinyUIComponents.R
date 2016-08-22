library(shiny)

#' Create the Shiny UI controls to choose the viewPoint
#' 
#' @export
viewPointUI <- function(coordLabels) {
  resultList <- list(p("view point"))
  for( aLabel in coordLabels) {
    resultList[[length(resultList)+1]] <- 
      sliderInput(
        paste("vp.", aLabel, sep = ""),
        "", -1000, 1000, 0)
  }
  inputPanel(resultList)
}

#' Create the Shiny UI controls to choose the viewAngle
#' 
#' @export
viewAngleUI <- function(coordLabels) {
  resultList <- list(p("viewing angles"))
  for( aLabel in coordLabels) {
    resultList[[length(resultList)+1]] <- 
      sliderInput(
        paste("va.", aLabel, sep = ""),
        "", -180, 180, 0)
  }
  inputPanel(resultList)
}