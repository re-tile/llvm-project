add_llvm_component_library(LLVMTileDesc
  TileInstPrinter.cpp
  TileMCAsmInfo.cpp
  TileMCTargetDesc.cpp

  LINK_COMPONENTS
  MC
  Support
  TargetParser
  TileInfo

  ADD_TO_COMPONENT
  Tile
  )
