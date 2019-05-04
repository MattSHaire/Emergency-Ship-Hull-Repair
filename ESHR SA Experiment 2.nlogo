;; EMERGENCY SHIP HULL REPAIR
;; SELF ASSEMBLING AGENTS APPROACH VERSION 3.1
;; BY MATTHEW HAIRE
;; LAST EDITED:    03 May 2019

globals
[
  seal  ;; check if breach is sealed or not
  speed ;; movement speed of turtles
  goalx ;; goal x coordinate
  goaly ;; goal y coordinate
  sproutx ;; x coordinates of turtle for creation
  increments ;; while loop variable for robots
  spacing ;; spacing for robots
  sub_breach
  sub_agent
  total_ingress
  Q
  Area
  turtles_attached
  turtles_unattached
]

turtles-own
[
  active  ;; state of the turtle - either in transit or in position
  goalpos ;; goal position of turtle - either centre, left or right or breach
  agent_ahead ;; reporter for color of agent ahead of turtle
]

to setup
  clear-all
  reset-ticks
  set seal 0
  set speed 1
  set turtles_attached 0
  set turtles_unattached 0
  setup-environment
  setup-turtles
end

to setup-environment
  resize-world -50 50 -50 50
  set-patch-size 5
  set goalx breachx
  set goaly (breachy + breachsize + 1)
  set sub_agent 0
  set total_ingress 0
  set Q 0
  set Area (pi * breachsize ^ 2 * 0.00694)
  ask patches
  [
    set pcolor 5
    if pycor <= (min-pycor + (max-pycor * 1.9))
    [
      set pcolor 105
    ]
    ask patch breachx breachy
    [
      set pcolor 101
      ask patches in-radius breachsize
      [
        set pcolor 101
      ]
    ]
  ]
end

to setup-turtles

  set increments 0
  set sproutx 0
  set spacing (96 / robotpop)

  while [increments < (robotpop / 2)]
  [
    set sproutx (sproutx - spacing)
    ask patch sproutx 48
    [
      Sprout 1
    ]
    set increments (increments + 1)
  ]

  set sproutx 0
  while [increments < robotpop]
  [
    set sproutx (sproutx + spacing)
    ask patch sproutx 48
    [
      Sprout 1
    ]
    set increments (increments + 1)
  ]

  ask turtles
    [
      set active 1
      set goalpos "centre"
      set shape "square"
      set size 1
      set color red
      set agent_ahead "null"
    ]
end

to start
  if (ticks > 1) AND (remainder ticks mov_spd) = 0
  [
    water-ingress-calc
  ]
  if seal = 1 OR total_ingress >= 14870
  [
    set turtles_attached count turtles with [xcor >= (breachx - breachsize - 1) AND xcor <= (breachx + breachsize + 1) AND ycor >= (breachy - breachsize - 1) AND ycor <= (breachy + breachsize + 1)]

    if breachsize = 6 [set turtles_unattached count turtles - turtles_attached]
    if breachsize = 5 [set turtles_unattached count turtles - turtles_attached]
    if breachsize = 4 [set turtles_unattached count turtles - turtles_attached]
    if breachsize = 3 [set turtles_unattached count turtles - turtles_attached]
    if breachsize = 2 [set turtles_unattached count turtles - turtles_attached]
    if breachsize = 1 [set turtles_unattached count turtles - turtles_attached]

    show count turtles
    show turtles_unattached
    show total_ingress
    show seal
    stop
  ]
  water-ingress-calc
  turtle-actions
  spawn-turtles
  advance-line
  tick
end

to turtle-actions
  ask turtles
  [
    if active = 1
    [
      if goalpos = "left"
      [
        ifelse pycor > (goaly + 1)
        [
          setxy pxcor (goaly + 1)
        ]
        [
          ifelse not any? (turtles-on patch-left-and-ahead 90 1)
          [
            set agent_ahead "null"
            setxy pxcor goaly
            set color orange
            set active 0
            if pxcor > goalx
            [
              set goalpos "right"
            ]
          ]
          [
            set heading 270
            forward speed
          ]
          if pxcor < (goalx - breachsize - 1)
          [
            set goalpos "lost"
            set active 1
          ]
        ]
      ]
      if goalpos = "right"
      [
        ifelse pycor > (goaly + 1)
        [
          setxy pxcor (goaly + 1)
        ]
        [
          ifelse not any? (turtles-on patch-right-and-ahead 90 1)
          [
            set agent_ahead "null"
            setxy pxcor goaly
            set color orange
            set active 0
            if pxcor < goalx
            [
              set goalpos "left"
            ]
          ]
          [
            set heading 90
            forward speed
          ]
          if pxcor > (goalx + breachsize + 1)
          [
            set goalpos "lost"
            set active 1
          ]
        ]
      ]

      if goalpos = "lost"
      [
          facexy pxcor (goaly + 6)
          ifelse not any? (turtles-on patch-ahead 1.5)
          [
          set agent_ahead "null"
          ifelse pycor >= (goaly + 6)
           [
             set goalpos "centre"
             facexy goalx goaly
             set color red
           ]
           [
             forward speed
           ]
          ]
          [
           if any? (turtles-on patch-ahead 1.5) with [color = red]
           [
             set agent_ahead "red"
             back speed ;; previously 1
           ]
           if any? (turtles-on patch-ahead 1.5) with [color = orange]
           [
             set agent_ahead "orange"
             back speed ;; previously 1
           ]
           if any? (turtles-on patch-ahead 1.5) with [color = green]
           [
             set agent_ahead "green"
             back speed ;; previously 1
           ]
          ]
      ]

      if goalpos = "centre"
      [
        facexy goalx goaly
        ifelse not any? (turtles-on patch-ahead 1.5)
        [
          set agent_ahead "null"
          ifelse (pxcor < (goalx + 0.5) AND pxcor > (goalx - 0.5) AND pycor > (goaly - 0.5) AND pycor < (goaly + 0.5))
          [
            setxy goalx goaly
            set heading 180
            set color orange
            set active 0
          ]
          [
            forward speed ;; previously 1
          ]
        ]
        [
          if any? (turtles-on patch-ahead 1.5) with [color = red]
          [
            set agent_ahead "red"
            back speed
          ]
          if any? (turtles-on patch-ahead 1.5) with [color = orange]
          [
            set agent_ahead "orange"
            setxy pxcor pycor
            if not any? (turtles-on patch (goalx - 1) goaly) OR any? (turtles-on patch (goalx - 1) goaly) with [color = orange]
            [
              set heading 270
              ifelse any? (turtles-on patch-ahead 1) OR any? (turtles-on patch-ahead 2)
              [
                set heading 90
                set goalpos "right"
              ]
              [
                set heading 270
                set goalpos "left"
              ]
            ]
            if (not any? (turtles-on patch (goalx + 1) goaly) OR any? (turtles-on patch (goalx + 1) goaly) with [color = orange]) AND (goalpos != "left")
            [
              set heading 90
              set goalpos "right"
            ]
          ]
          if any? (turtles-on patch-ahead 1.5) with [color = green]
          [
            set agent_ahead "green"
            ifelse not any? (turtles-on patch goalx goaly)
            [
              setxy goalx goaly
              set heading 180
              set color orange
              set active 0
            ]
            [
              back (speed / 2)
            ]
          ]
        ]
      ]
    ]
    if active = 0
    [
      ifelse goalpos = "centre"
      [
        if any? ((turtles-on patch (goalx - 1) goaly) with [color = green]) AND any? ((turtles-on patch (goalx + 1) goaly) with [color = green])
        [
          set color green
        ]
      ]
      [
        ifelse xcor < (goalx - breachsize) OR xcor > (goalx + breachsize)
        [
          set color green
        ]
        [
          if any? (turtles-on patch (pxcor - 1) pycor) with [color = green] OR any? (turtles-on patch (pxcor + 1) pycor) with [color = green]
          [
            set color green
          ]
        ]
      ]
    ]
  ]
end

to spawn-turtles
  if (ticks > 1) AND (remainder ticks Deployrate) = 0 [
  set increments 0
  set sproutx 0
  while [increments < (robotpop / 2)]
  [
    set sproutx (sproutx - spacing)
    ask patch sproutx 48
    [
      Sprout 1
    ]
    set increments (increments + 1)
  ]

  set sproutx 0
  while [increments < robotpop]
  [
    set sproutx (sproutx + spacing)
    ask patch sproutx 48
    [
      Sprout 1
    ]
    set increments (increments + 1)
  ]

    ask turtles with [pycor = 48]
    [
      set active 1
      set goalpos "centre"
      set shape "square"
      set size 1
      set color red
      set agent_ahead "null"
    ]
  ]
end

to advance-line
  if any? (turtles-on patch breachx (breachy - breachsize - 1)) AND any? (turtles-on patch goalx goaly) with [color = green]
  [
    ask turtles
    [
      set color violet
      set active 3
    ]
    set seal 1
  ]
  if any? (turtles-on patch goalx goaly) with [color = green]
  [
    ask turtles with [ycor < (goaly + 0.5)]
    [
      set heading 180
      forward speed
      set active 3
    ]
  ]
end

to water-ingress-calc
  if breachy = 33 [set Q (Area * sqrt (64.348 * 1))]
  if breachy = 21 [set Q (Area * sqrt (64.348 * 2))]
  if breachy = 9 [set Q (Area * sqrt (64.348 * 3))]
  if breachy = -3 [set Q (Area * sqrt (64.348 * 4))]
  if breachy = -15 [set Q (Area * sqrt (64.348 * 5))]
  if breachy = -27 [set Q (Area * sqrt (64.348 * 6))]
  if breachy = -39 [set Q (Area * sqrt (64.348 * 7))]

  set sub_breach (count patches with [pcolor = 101])
  set sub_agent (count turtles-on patches with [pcolor = 101])
  set Q (Q * (sub_breach - sub_agent) / sub_breach)
  set Q (Q / 60 * 6.23)
  set total_ingress (total_ingress + Q)
end











@#$#@#$#@
GRAPHICS-WINDOW
210
10
723
524
-1
-1
5.0
1
10
1
1
1
0
0
0
1
-50
50
-50
50
1
1
1
ticks
30.0

BUTTON
13
16
91
49
SETUP
setup
NIL
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

BUTTON
100
16
175
49
START
start
T
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

CHOOSER
23
59
161
104
breachsize
breachsize
1 2 3 4 5 6
3

SLIDER
7
128
179
161
breachx
breachx
-40
40
9.0
1
1
NIL
HORIZONTAL

SLIDER
8
175
180
208
breachy
breachy
-39
33
-3.0
6
1
NIL
HORIZONTAL

SLIDER
8
222
180
255
robotpop
robotpop
2
48
10.0
2
1
Robots
HORIZONTAL

SLIDER
12
275
184
308
Deployrate
Deployrate
15
120
60.0
15
1
seconds
HORIZONTAL

CHOOSER
23
332
161
377
mov_spd
mov_spd
1 2 3
0

@#$#@#$#@
## WHAT IS IT?

A Novel approach to emergancy ship hull repair using a swarm of self-assembling repair robots.

## HOW IT WORKS

The ship hull is lined with cables that can detect breaks which indicates a hull breach. The ship IPMS records the location of the breach, and relays the point to a collection of repair robots - signalling them to approach and repair the breach using their bodies. All repair robots use the same algorithm, improving scalability and robustness of the system - which are two of the main advantages of swarm robotics solutions.

## HOW TO USE IT

Use the sliders to select the position of the hull breach which will always be below the waterline. Use the breachsize selector to choose your desired radius of the hull breach. use the setup button to initialise the environment and repair robots. Press the start button to begin the simulation and watch it run.

## THINGS TO NOTICE

Pay attention to the behaviour of the robots when they appraoch the breach. All agents are coloured acording to their LED colour; in a physical system these colours help facilitate communication between repair robots. RED represents an agent in transit, ORANGE indicates an agent in position and waiting for its neighbours to align, and GREEN rpresents readiness to advance to next alignment. Once the breach is sealed, all agents change to purple LED colours.

## THINGS TO TRY

Try changing the size and position of the breach, see how the agents react and how long it takes for the repair robots to seal the breach.

## EXTENDING THE MODEL

A good extension to this model would be modification of the ease of movement of agents. Agents are operating underwater and this exerts drag and lift forces on the repair robots. Implementing these forces into the model would improve the accuracy of the model and help reduce the reality gap.

## NETLOGO FEATURES

(interesting or unusual features of NetLogo that the model uses, particularly in the Code tab; or where workarounds were needed for missing features)

## RELATED MODELS

See alternate approach to emergancy ship hull repair using a swarm of repair robots and a repair sheet; diferences in robustness, scalibility and flexibility of system.

## CREDITS AND REFERENCES

(a reference to the model's URL on the web if it has one, as well as any other necessary credits, citations, and links)
@#$#@#$#@
default
true
0
Polygon -7500403 true true 150 5 40 250 150 205 260 250

airplane
true
0
Polygon -7500403 true true 150 0 135 15 120 60 120 105 15 165 15 195 120 180 135 240 105 270 120 285 150 270 180 285 210 270 165 240 180 180 285 195 285 165 180 105 180 60 165 15

arrow
true
0
Polygon -7500403 true true 150 0 0 150 105 150 105 293 195 293 195 150 300 150

box
false
0
Polygon -7500403 true true 150 285 285 225 285 75 150 135
Polygon -7500403 true true 150 135 15 75 150 15 285 75
Polygon -7500403 true true 15 75 15 225 150 285 150 135
Line -16777216 false 150 285 150 135
Line -16777216 false 150 135 15 75
Line -16777216 false 150 135 285 75

bug
true
0
Circle -7500403 true true 96 182 108
Circle -7500403 true true 110 127 80
Circle -7500403 true true 110 75 80
Line -7500403 true 150 100 80 30
Line -7500403 true 150 100 220 30

butterfly
true
0
Polygon -7500403 true true 150 165 209 199 225 225 225 255 195 270 165 255 150 240
Polygon -7500403 true true 150 165 89 198 75 225 75 255 105 270 135 255 150 240
Polygon -7500403 true true 139 148 100 105 55 90 25 90 10 105 10 135 25 180 40 195 85 194 139 163
Polygon -7500403 true true 162 150 200 105 245 90 275 90 290 105 290 135 275 180 260 195 215 195 162 165
Polygon -16777216 true false 150 255 135 225 120 150 135 120 150 105 165 120 180 150 165 225
Circle -16777216 true false 135 90 30
Line -16777216 false 150 105 195 60
Line -16777216 false 150 105 105 60

car
false
0
Polygon -7500403 true true 300 180 279 164 261 144 240 135 226 132 213 106 203 84 185 63 159 50 135 50 75 60 0 150 0 165 0 225 300 225 300 180
Circle -16777216 true false 180 180 90
Circle -16777216 true false 30 180 90
Polygon -16777216 true false 162 80 132 78 134 135 209 135 194 105 189 96 180 89
Circle -7500403 true true 47 195 58
Circle -7500403 true true 195 195 58

circle
false
0
Circle -7500403 true true 0 0 300

circle 2
false
0
Circle -7500403 true true 0 0 300
Circle -16777216 true false 30 30 240

cow
false
0
Polygon -7500403 true true 200 193 197 249 179 249 177 196 166 187 140 189 93 191 78 179 72 211 49 209 48 181 37 149 25 120 25 89 45 72 103 84 179 75 198 76 252 64 272 81 293 103 285 121 255 121 242 118 224 167
Polygon -7500403 true true 73 210 86 251 62 249 48 208
Polygon -7500403 true true 25 114 16 195 9 204 23 213 25 200 39 123

cylinder
false
0
Circle -7500403 true true 0 0 300

dot
false
0
Circle -7500403 true true 90 90 120

face happy
false
0
Circle -7500403 true true 8 8 285
Circle -16777216 true false 60 75 60
Circle -16777216 true false 180 75 60
Polygon -16777216 true false 150 255 90 239 62 213 47 191 67 179 90 203 109 218 150 225 192 218 210 203 227 181 251 194 236 217 212 240

face neutral
false
0
Circle -7500403 true true 8 7 285
Circle -16777216 true false 60 75 60
Circle -16777216 true false 180 75 60
Rectangle -16777216 true false 60 195 240 225

face sad
false
0
Circle -7500403 true true 8 8 285
Circle -16777216 true false 60 75 60
Circle -16777216 true false 180 75 60
Polygon -16777216 true false 150 168 90 184 62 210 47 232 67 244 90 220 109 205 150 198 192 205 210 220 227 242 251 229 236 206 212 183

fish
false
0
Polygon -1 true false 44 131 21 87 15 86 0 120 15 150 0 180 13 214 20 212 45 166
Polygon -1 true false 135 195 119 235 95 218 76 210 46 204 60 165
Polygon -1 true false 75 45 83 77 71 103 86 114 166 78 135 60
Polygon -7500403 true true 30 136 151 77 226 81 280 119 292 146 292 160 287 170 270 195 195 210 151 212 30 166
Circle -16777216 true false 215 106 30

flag
false
0
Rectangle -7500403 true true 60 15 75 300
Polygon -7500403 true true 90 150 270 90 90 30
Line -7500403 true 75 135 90 135
Line -7500403 true 75 45 90 45

flower
false
0
Polygon -10899396 true false 135 120 165 165 180 210 180 240 150 300 165 300 195 240 195 195 165 135
Circle -7500403 true true 85 132 38
Circle -7500403 true true 130 147 38
Circle -7500403 true true 192 85 38
Circle -7500403 true true 85 40 38
Circle -7500403 true true 177 40 38
Circle -7500403 true true 177 132 38
Circle -7500403 true true 70 85 38
Circle -7500403 true true 130 25 38
Circle -7500403 true true 96 51 108
Circle -16777216 true false 113 68 74
Polygon -10899396 true false 189 233 219 188 249 173 279 188 234 218
Polygon -10899396 true false 180 255 150 210 105 210 75 240 135 240

house
false
0
Rectangle -7500403 true true 45 120 255 285
Rectangle -16777216 true false 120 210 180 285
Polygon -7500403 true true 15 120 150 15 285 120
Line -16777216 false 30 120 270 120

leaf
false
0
Polygon -7500403 true true 150 210 135 195 120 210 60 210 30 195 60 180 60 165 15 135 30 120 15 105 40 104 45 90 60 90 90 105 105 120 120 120 105 60 120 60 135 30 150 15 165 30 180 60 195 60 180 120 195 120 210 105 240 90 255 90 263 104 285 105 270 120 285 135 240 165 240 180 270 195 240 210 180 210 165 195
Polygon -7500403 true true 135 195 135 240 120 255 105 255 105 285 135 285 165 240 165 195

line
true
0
Line -7500403 true 150 0 150 300

line half
true
0
Line -7500403 true 150 0 150 150

pentagon
false
0
Polygon -7500403 true true 150 15 15 120 60 285 240 285 285 120

person
false
0
Circle -7500403 true true 110 5 80
Polygon -7500403 true true 105 90 120 195 90 285 105 300 135 300 150 225 165 300 195 300 210 285 180 195 195 90
Rectangle -7500403 true true 127 79 172 94
Polygon -7500403 true true 195 90 240 150 225 180 165 105
Polygon -7500403 true true 105 90 60 150 75 180 135 105

plant
false
0
Rectangle -7500403 true true 135 90 165 300
Polygon -7500403 true true 135 255 90 210 45 195 75 255 135 285
Polygon -7500403 true true 165 255 210 210 255 195 225 255 165 285
Polygon -7500403 true true 135 180 90 135 45 120 75 180 135 210
Polygon -7500403 true true 165 180 165 210 225 180 255 120 210 135
Polygon -7500403 true true 135 105 90 60 45 45 75 105 135 135
Polygon -7500403 true true 165 105 165 135 225 105 255 45 210 60
Polygon -7500403 true true 135 90 120 45 150 15 180 45 165 90

sheep
false
15
Circle -1 true true 203 65 88
Circle -1 true true 70 65 162
Circle -1 true true 150 105 120
Polygon -7500403 true false 218 120 240 165 255 165 278 120
Circle -7500403 true false 214 72 67
Rectangle -1 true true 164 223 179 298
Polygon -1 true true 45 285 30 285 30 240 15 195 45 210
Circle -1 true true 3 83 150
Rectangle -1 true true 65 221 80 296
Polygon -1 true true 195 285 210 285 210 240 240 210 195 210
Polygon -7500403 true false 276 85 285 105 302 99 294 83
Polygon -7500403 true false 219 85 210 105 193 99 201 83

square
false
0
Rectangle -7500403 true true 30 30 270 270

square 2
false
0
Rectangle -7500403 true true 30 30 270 270
Rectangle -16777216 true false 60 60 240 240

star
false
0
Polygon -7500403 true true 151 1 185 108 298 108 207 175 242 282 151 216 59 282 94 175 3 108 116 108

target
false
0
Circle -7500403 true true 0 0 300
Circle -16777216 true false 30 30 240
Circle -7500403 true true 60 60 180
Circle -16777216 true false 90 90 120
Circle -7500403 true true 120 120 60

tree
false
0
Circle -7500403 true true 118 3 94
Rectangle -6459832 true false 120 195 180 300
Circle -7500403 true true 65 21 108
Circle -7500403 true true 116 41 127
Circle -7500403 true true 45 90 120
Circle -7500403 true true 104 74 152

triangle
false
0
Polygon -7500403 true true 150 30 15 255 285 255

triangle 2
false
0
Polygon -7500403 true true 150 30 15 255 285 255
Polygon -16777216 true false 151 99 225 223 75 224

truck
false
0
Rectangle -7500403 true true 4 45 195 187
Polygon -7500403 true true 296 193 296 150 259 134 244 104 208 104 207 194
Rectangle -1 true false 195 60 195 105
Polygon -16777216 true false 238 112 252 141 219 141 218 112
Circle -16777216 true false 234 174 42
Rectangle -7500403 true true 181 185 214 194
Circle -16777216 true false 144 174 42
Circle -16777216 true false 24 174 42
Circle -7500403 false true 24 174 42
Circle -7500403 false true 144 174 42
Circle -7500403 false true 234 174 42

turtle
true
0
Polygon -10899396 true false 215 204 240 233 246 254 228 266 215 252 193 210
Polygon -10899396 true false 195 90 225 75 245 75 260 89 269 108 261 124 240 105 225 105 210 105
Polygon -10899396 true false 105 90 75 75 55 75 40 89 31 108 39 124 60 105 75 105 90 105
Polygon -10899396 true false 132 85 134 64 107 51 108 17 150 2 192 18 192 52 169 65 172 87
Polygon -10899396 true false 85 204 60 233 54 254 72 266 85 252 107 210
Polygon -7500403 true true 119 75 179 75 209 101 224 135 220 225 175 261 128 261 81 224 74 135 88 99

wheel
false
0
Circle -7500403 true true 3 3 294
Circle -16777216 true false 30 30 240
Line -7500403 true 150 285 150 15
Line -7500403 true 15 150 285 150
Circle -7500403 true true 120 120 60
Line -7500403 true 216 40 79 269
Line -7500403 true 40 84 269 221
Line -7500403 true 40 216 269 79
Line -7500403 true 84 40 221 269

wolf
false
0
Polygon -16777216 true false 253 133 245 131 245 133
Polygon -7500403 true true 2 194 13 197 30 191 38 193 38 205 20 226 20 257 27 265 38 266 40 260 31 253 31 230 60 206 68 198 75 209 66 228 65 243 82 261 84 268 100 267 103 261 77 239 79 231 100 207 98 196 119 201 143 202 160 195 166 210 172 213 173 238 167 251 160 248 154 265 169 264 178 247 186 240 198 260 200 271 217 271 219 262 207 258 195 230 192 198 210 184 227 164 242 144 259 145 284 151 277 141 293 140 299 134 297 127 273 119 270 105
Polygon -7500403 true true -1 195 14 180 36 166 40 153 53 140 82 131 134 133 159 126 188 115 227 108 236 102 238 98 268 86 269 92 281 87 269 103 269 113

x
false
0
Polygon -7500403 true true 270 75 225 30 30 225 75 270
Polygon -7500403 true true 30 75 75 30 270 225 225 270
@#$#@#$#@
NetLogo 6.0.2
@#$#@#$#@
@#$#@#$#@
@#$#@#$#@
<experiments>
  <experiment name="Size 1 Ingress" repetitions="50" runMetricsEveryStep="false">
    <setup>SETUP</setup>
    <go>START</go>
    <timeLimit steps="1000"/>
    <metric>seal</metric>
    <metric>turtles_unattached</metric>
    <metric>total_ingress</metric>
    <enumeratedValueSet variable="breachsize">
      <value value="1"/>
    </enumeratedValueSet>
    <enumeratedValueSet variable="Deployrate">
      <value value="60"/>
    </enumeratedValueSet>
    <enumeratedValueSet variable="breachx">
      <value value="0"/>
    </enumeratedValueSet>
    <enumeratedValueSet variable="mov_spd">
      <value value="1"/>
    </enumeratedValueSet>
    <steppedValueSet variable="breachy" first="-39" step="12" last="33"/>
    <steppedValueSet variable="robotpop" first="2" step="2" last="48"/>
  </experiment>
  <experiment name="Size 2 Ingress" repetitions="50" runMetricsEveryStep="false">
    <setup>SETUP</setup>
    <go>START</go>
    <timeLimit steps="1000"/>
    <metric>seal</metric>
    <metric>turtles_unattached</metric>
    <metric>total_ingress</metric>
    <enumeratedValueSet variable="breachsize">
      <value value="2"/>
    </enumeratedValueSet>
    <enumeratedValueSet variable="Deployrate">
      <value value="60"/>
    </enumeratedValueSet>
    <enumeratedValueSet variable="breachx">
      <value value="0"/>
    </enumeratedValueSet>
    <enumeratedValueSet variable="mov_spd">
      <value value="1"/>
    </enumeratedValueSet>
    <steppedValueSet variable="breachy" first="-39" step="12" last="33"/>
    <steppedValueSet variable="robotpop" first="2" step="2" last="48"/>
  </experiment>
  <experiment name="Size 3 Ingress" repetitions="50" runMetricsEveryStep="false">
    <setup>SETUP</setup>
    <go>START</go>
    <timeLimit steps="1000"/>
    <metric>seal</metric>
    <metric>turtles_unattached</metric>
    <metric>total_ingress</metric>
    <enumeratedValueSet variable="breachsize">
      <value value="3"/>
    </enumeratedValueSet>
    <enumeratedValueSet variable="Deployrate">
      <value value="60"/>
    </enumeratedValueSet>
    <enumeratedValueSet variable="breachx">
      <value value="0"/>
    </enumeratedValueSet>
    <enumeratedValueSet variable="mov_spd">
      <value value="1"/>
    </enumeratedValueSet>
    <steppedValueSet variable="breachy" first="-39" step="12" last="33"/>
    <steppedValueSet variable="robotpop" first="2" step="2" last="48"/>
  </experiment>
  <experiment name="Size 4 Ingress" repetitions="50" runMetricsEveryStep="false">
    <setup>SETUP</setup>
    <go>START</go>
    <timeLimit steps="1000"/>
    <metric>seal</metric>
    <metric>turtles_unattached</metric>
    <metric>total_ingress</metric>
    <enumeratedValueSet variable="breachsize">
      <value value="4"/>
    </enumeratedValueSet>
    <enumeratedValueSet variable="Deployrate">
      <value value="60"/>
    </enumeratedValueSet>
    <enumeratedValueSet variable="breachx">
      <value value="0"/>
    </enumeratedValueSet>
    <enumeratedValueSet variable="mov_spd">
      <value value="1"/>
    </enumeratedValueSet>
    <steppedValueSet variable="breachy" first="-39" step="12" last="33"/>
    <steppedValueSet variable="robotpop" first="2" step="2" last="48"/>
  </experiment>
  <experiment name="Size 5 Ingress" repetitions="50" runMetricsEveryStep="false">
    <setup>SETUP</setup>
    <go>START</go>
    <timeLimit steps="1000"/>
    <metric>seal</metric>
    <metric>turtles_unattached</metric>
    <metric>total_ingress</metric>
    <enumeratedValueSet variable="breachsize">
      <value value="5"/>
    </enumeratedValueSet>
    <enumeratedValueSet variable="Deployrate">
      <value value="60"/>
    </enumeratedValueSet>
    <enumeratedValueSet variable="breachx">
      <value value="0"/>
    </enumeratedValueSet>
    <enumeratedValueSet variable="mov_spd">
      <value value="1"/>
    </enumeratedValueSet>
    <steppedValueSet variable="breachy" first="-39" step="12" last="33"/>
    <steppedValueSet variable="robotpop" first="2" step="2" last="48"/>
  </experiment>
  <experiment name="Size 6 Ingress" repetitions="50" runMetricsEveryStep="false">
    <setup>SETUP</setup>
    <go>START</go>
    <timeLimit steps="1000"/>
    <metric>seal</metric>
    <metric>turtles_unattached</metric>
    <metric>total_ingress</metric>
    <enumeratedValueSet variable="breachsize">
      <value value="6"/>
    </enumeratedValueSet>
    <enumeratedValueSet variable="Deployrate">
      <value value="60"/>
    </enumeratedValueSet>
    <enumeratedValueSet variable="breachx">
      <value value="0"/>
    </enumeratedValueSet>
    <enumeratedValueSet variable="mov_spd">
      <value value="1"/>
    </enumeratedValueSet>
    <steppedValueSet variable="breachy" first="-39" step="12" last="33"/>
    <steppedValueSet variable="robotpop" first="2" step="2" last="48"/>
  </experiment>
</experiments>
@#$#@#$#@
@#$#@#$#@
default
0.0
-0.2 0 0.0 1.0
0.0 1 1.0 0.0
0.2 0 0.0 1.0
link direction
true
0
Line -7500403 true 150 150 90 180
Line -7500403 true 150 150 210 180
@#$#@#$#@
0
@#$#@#$#@
