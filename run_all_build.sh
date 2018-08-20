run_build() (
echo "===== Building $BUILD_CONFIG ======"
cd $BUILD_CONFIG
cmake --build . -- -j4 
if [[ $? != 0 ]]; then
  echo "***** Failed $BUILD_CONFIG *****" 
  cd ..
  exit 1 
else
  echo "===== Complete $BUILD_CONFIG ======"
  cd ..
fi
)

BUILD_CONFIG=build_mac_debug
run_build || exit 1

BUILD_CONFIG=build_mac_rel
run_build || exit 1

BUILD_CONFIG=build_ios_debug
run_build || exit 1

BUILD_CONFIG=build_ios_rel
run_build || exit 1

BUILD_CONFIG=build_ios_simulator_debug
run_build || exit 1

BUILD_CONFIG=build_ios_simulator_rel
run_build || exit 1
