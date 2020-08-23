# Recording Parser

## Data Analysis

Here's a useful gnuplot palette that shows big differences for the last 4 out of 26 values.
```gnuplot
set palette model HSV defined ( 0 0 0 1, 2 0 0.5 1, 3 0 1 1, 3.5 1 1 0.2)
p 'file' u 1:2:0 with points pt 5 ps 3 palette
```

After visual inspection I think it's best to remove the last 3 points for Sony A7R photos (10ms between recordings). The shutter seems to affect the gravity recording. This was tested on a tripod with 2s delay for every photo so the problem can't be due to me shaking the camera while pressing the trigger.

### Effect of Shaky Camera

Sony a7sII, e front curtain off, Minolta 50mm lens, shaky handheld. Worst example in series of 12 photos.

Std excluding last 5 frames: 0.082219   0.113727   0.016734

Diameter of plot (visually) roughly 11deg

```gnuplot
set palette model HSV defined ( 0 0 0 0.6, 2 0 0.5 1, 3 0 1 1, 3.5 1 1 0.2)
p '009' u 1:2:0 with lines palette t "Sony a7sII handheld shaky"
```

```
 0.143899 -0.104604 -0.984048
 0.215733 -0.177248  -0.96023
 0.198328 -0.227948 -0.953261
 0.151226 -0.209839  -0.96597
0.0713005 -0.126326 -0.989423
0.0658967 0.000260461   -0.997826
0.0965698  0.091059 -0.991152
 0.178952  0.102372 -0.978517
 0.274644 0.0286143  -0.96112
 0.307444 -0.0894383  -0.947354
 0.215563 -0.184018 -0.958994
 0.150395 -0.168043  -0.97424
 0.0926461 -0.0752276  -0.992853
0.0356903 -0.00815779    -0.99933
0.0253039 -0.00971262   -0.999633
0.0400456 -0.086807  -0.99542
0.0563373 -0.220347 -0.973793
 0.075968  -0.25819 -0.963103
0.0845338 -0.277069 -0.957124
0.0655854 -0.240648 -0.968394
0.0381905  -0.16432 -0.985668
 0.039702 -0.101812 -0.994011
 0.0294451 -0.0644873  -0.997484
 0.0760183 -0.0400824  -0.996301
 0.039877 -0.0957049  -0.994611
0.0294537 -0.209063 -0.977458
```

### Expected Normal Case

Sony a7sII, e front curtain on, Minolta 50mm lens, normal handheld. Worst example in series of 6 photos.

Std excluding last 5 frames: 0.0147640   0.0086168   0.0045262

Diameter of plot (visually) roughly 2deg

```
0.0272192 -0.475948 -0.879052
0.0287551 -0.470703 -0.881823
0.00750038  -0.472266  -0.881424
-0.00411264   -0.471411   -0.881904
-0.00361978    -0.47807   -0.878314
0.00548447  -0.482373  -0.875949
 0.018922 -0.480147 -0.876984
0.0146906 -0.474033 -0.880385
0.0173772 -0.463917 -0.885708
0.00818893  -0.457524   -0.88916
-0.014445 -0.449372 -0.893228
-0.0238656  -0.455781  -0.889772
-0.0203602  -0.463131  -0.886056
-0.00595173   -0.470446   -0.882409
0.00233211  -0.481191  -0.876613
0.000258514   -0.476441   -0.879206
-0.0100998  -0.474431  -0.880235
-0.00644569   -0.472598   -0.881255
0.00388196   -0.47127  -0.881981
0.0144718  -0.46956 -0.882782
0.0211049 -0.477595 -0.878327
0.0166215 -0.479168 -0.877566
0.0133483 -0.479231 -0.877587
  0.01254 -0.478869 -0.877797
0.0103631 -0.478518 -0.878017
-0.00130162   -0.476132   -0.879373
```


### On Table, self timer

Sony a7sII, e front curtain on, Minolta 50mm lens, on table with 2s self timer. Worst example in series of 4 photos.

Std excluding last 5 frames: 0.0041189   0.0030978   0.0013623

Diameter of plot (visually) roughly 1deg

```
 0.167158 -0.319735 -0.932646
  0.16819 -0.322257 -0.931592
 0.162046 -0.321019 -0.933107
 0.162307 -0.325648 -0.931456
 0.153183 -0.324903 -0.933259
 0.157555 -0.325167 -0.932439
 0.160547 -0.323929  -0.93236
 0.170559 -0.332616 -0.927511
 0.167331 -0.327938 -0.929762
 0.166623  -0.32037 -0.932523
 0.158081  -0.32622 -0.931982
 0.161268 -0.324848 -0.931915
 0.160517 -0.319229 -0.933985
 0.161374 -0.325584  -0.93164
 0.160623 -0.320988 -0.933363
 0.164975 -0.323215 -0.931834
 0.158739 -0.323405 -0.932851
 0.160461 -0.325839 -0.931709
 0.160745 -0.326891 -0.931291
0.165164 -0.32365 -0.93165
 0.163154 -0.325532 -0.931348
 0.168212  -0.32662 -0.930067
 0.163904 -0.322413 -0.932301
 0.161486 -0.326072  -0.93145
  0.15625 -0.323883 -0.933105
 0.158986 -0.325999 -0.931906
```

### Nice visual example of shutter vibration

Sony a7sII, heavy lens (Sigma 24-70mm F2.8 DG DN Art), handheld

```
 0.0163752 -0.0844755  -0.996291
 0.0257631 -0.0855336  -0.996002
 0.0297949 -0.0880892  -0.995667
 0.033177 -0.085016 -0.995827
0.0311525 -0.082554   -0.9961
 0.0304261 -0.0808762   -0.99626
 0.0175868 -0.0848304   -0.99624
 0.0147875 -0.0706232  -0.997393
 0.0192635 -0.0773108  -0.996821
 0.0181769 -0.0729636  -0.997169
 0.0234512 -0.0693227  -0.997319
 0.0214317 -0.0748819  -0.996962
0.00748257 -0.0737936  -0.997245
0.00921031 -0.0757877  -0.997081
0.00255204 -0.0763059  -0.997081
0.00745873 -0.0889719  -0.996006
 0.0263643 -0.0616873  -0.997747
  0.031447 -0.0794037  -0.996346
 0.0347439 -0.0540178  -0.997935
 0.0417674 -0.0825036  -0.995715
 0.0160022 -0.0505877  -0.998591
0.0478785 -0.124534  -0.99106
0.00669243 -0.0447878  -0.998974
-0.0142357  -0.112543  -0.993545
-0.0485026  0.0980032  -0.994004
0.0481404 -0.136974 -0.989404
```
