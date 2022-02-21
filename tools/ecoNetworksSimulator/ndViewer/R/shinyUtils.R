# Report some of the important current graphics parameters
#
#' @export
.R_graphicsParameters <- function() {
  cat(file=stderr(), "part(din) = ", par("din"), "\n")
  cat(file=stderr(), "part(fig) = ", par("fig"), "\n")
  cat(file=stderr(), "part(fin) = ", par("fin"), "\n")
  cat(file=stderr(), "part(plt) = ", par("plt"), "\n")
  cat(file=stderr(), "part(pin) = ", par("pin"), "\n")
  cat(file=stderr(), "part(pty) = ", par("pty"), "\n")
  cat(file=stderr(), "part(usr) = ", par("usr"), "\n")
  cat(file=stderr(), "part(xpd) = ", par("xpd"), "\n")
}