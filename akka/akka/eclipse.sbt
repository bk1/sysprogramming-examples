// Actually IDE specific settings belong into ~/.sbt/,
// but in order to ease the setup for the training we put the following here:

EclipseKeys.createSrc := EclipseCreateSrc.Default + EclipseCreateSrc.Resource

EclipseKeys.eclipseOutput := Some(".target")

EclipseKeys.withSource := true
