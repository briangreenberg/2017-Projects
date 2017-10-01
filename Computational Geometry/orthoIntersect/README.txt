README

Lab 3: Ortho-line search algorithm...

Overview:
Running intersect x will randomly generated x orthogonal line segments.
Then, a sweep line will pass through the window and iteratively compute intersections.
Segments will be highlighted in red as they are included in the active structure.
Intersections will be marked by a blue x.

Keypress options:
- Pressing r will clear the intersections and reset the sweep line; it will then draw again on the same segments.
- Pressing buttons 1-5 will run additional test cases... See code for details on each test case
- Our test case (5) is supposed to test whether two lines that reach a perpendicular intersection...
where they touch but do not cross through each other are consistently marked intersections or not.
- Pressing i or any other unmapped button will re-generate random segments and recompute.
- Pressing q will close the window

Known issues:
- Our test case (5) seems to run with inconsistencies.
- Sometimes the perpendicular intersections are added, sometimes they are ignored...
We are currently working on fixing this issue.


- Brian Greenberg and Osborn Ngimor
