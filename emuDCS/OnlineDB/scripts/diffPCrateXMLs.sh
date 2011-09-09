#!/bin/bash

echo "Diffing PeripheralCrate..."
grep '<Periph' $1 | sort -d > x2
grep '<Periph' $2 | sort -d > x1
diff x1 x2

echo "Diffing MPC..."
grep '<MPC' $1 | sort -d > x2
grep '<MPC' $2 | sort -d > x1
diff x1 x2

echo "Diffing CCB..."
grep '<CCB' $1 | sort -d > x2
grep '<CCB' $2 | sort -d > x1
diff x1 x2

echo "Diffing VCC..."
grep '<VCC' $1 | sort -d > x2
grep '<VCC' $2 | sort -d > x1
diff x1 x2

echo "Diffing CSC..."
grep '<CSC' $1 | sort -d > x2
grep '<CSC' $2 | sort -d > x1
diff x1 x2

echo "Diffing DAQMB..."
grep '<DAQMB' $1 | sort -d > x2
grep '<DAQMB' $2 | sort -d > x1
diff x1 x2

echo "Diffing CFEB..."
grep '<CFEB' $1 | sort -d > x2
grep '<CFEB' $2 | sort -d > x1
diff x1 x2

echo "Diffing TMB..."
grep '<TMB' $1 | sort -d > x2
grep '<TMB' $2 | sort -d > x1
diff x1 x2

echo "Diffing ALCT..."
grep '<ALCT' $1 | sort -d > x2
grep '<ALCT' $2 | sort -d > x1
diff x1 x2

echo "Diffing AnodeChannel..."
grep '<AnodeChannel' $1 | sort -d > x2
grep '<AnodeChannel' $2 | sort -d > x1
diff x1 x2

rm x1 x2

