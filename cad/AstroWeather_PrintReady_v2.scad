/*
  SPDX-License-Identifier: AGPL-3.0-or-later
  Copyright (C) 2026 Tarun and contributors

 AstroWeather India - K1C/K2 print-ready mechanical revision 2

 Every exported STL is placed in its intended print orientation.
 No supports are intended with a 0.4 mm nozzle and 0.20 mm layers.

  1 A_Electronics_Box
  2 B_Box_Lid
  3 C_Shield_Base
  4 D_Shield_Louver (print 6)
  5 E_Shield_Roof
  6 F_Shield_Rod (print 3)
  7 G_SHT31_Sled
  8 H_Sky_Head_Top
  9 I_Sky_Head_Tray
 10 J_Rain_Plate_Mount
 11 K_Mounting_Plate
 12 L_Fit_Kit_4_PIECES (print first)
 90 exploded assembly preview
*/

$fn = 64;
part_id = 90;
fit = 0.35;
wall = 2.4;
lid_clearance_per_side = 0.30;
peg_d = 3.60;
// H5 socket is deliberately larger than the legacy friction socket. The
// current split snap pin provides retention without relying on an undersized
// printed hole.
peg_clearance_d = 0.30;
board_clearance = 0.50;

module rr2d(s=[20,20],r=3) {
    hull()
        for (x=[r,s[0]-r], y=[r,s[1]-r])
            translate([x,y]) circle(r=r);
}

module rrbox(s=[20,20,5],r=3) {
    linear_extrude(s[2]) rr2d([s[0],s[1]],r);
}

module mark(txt, p=[0,0,0], size=5, h=0.55) {
    if (is_undef($show_part_labels) || $show_part_labels)
        translate(p) linear_extrude(h)
            text(txt,size=size,font="Liberation Sans:style=Bold",
                 halign="center",valign="center");
}

module A_box() {
    difference() {
        rrbox([120,80,42],5);
        translate([wall,wall,3])
            rrbox([120-2*wall,80-2*wall,42],3.2);
        // Down-facing cable entries when installed vertically.
        translate([20,-1,13]) rotate([-90,0,0]) cylinder(d=8,h=wall+2);
        translate([100,-1,13]) rotate([-90,0,0]) cylinder(d=8,h=wall+2);
        for (x=[25:22:91])
            translate([x,15,-0.1]) cube([12,3,4]);
    }
    // Generic ESP32/perfboard rails.
    for (y=[22,56]) translate([25,y,3]) cube([70,2.2,4]);
    // Cable anchors.
    for (x=[16,104])
        translate([x,12,3]) difference() {
            cube([8,8,6],center=true);
            rotate([90,0,0]) cylinder(d=3.5,h=10,center=true);
        }
    mark("A  BOX",[60,40,3],6);
}

module B_lid() {
    union() {
        rrbox([124,84,3],5.5);
        // Friction plug. Sand lightly if the fit coupon is tight.
        translate([(124-(115.2-2*lid_clearance_per_side))/2,
                   (84-(75.2-2*lid_clearance_per_side))/2,3])
            difference() {
                rrbox([115.2-2*lid_clearance_per_side,
                       75.2-2*lid_clearance_per_side,4],3);
                translate([2.4,2.4,-0.1])
                    rrbox([110.4-2*lid_clearance_per_side,
                           70.4-2*lid_clearance_per_side,4.2],2);
            }
        mark("B  LID",[62,42,3],6);
    }
}

module C_shield_base() {
    difference() {
      union() {
        difference() {
            cylinder(d=70,h=3);
            translate([0,0,-0.1]) cylinder(d=38,h=3.2);
        }
        // Crossbars and sled guides.
        translate([-31,-2,0]) cube([62,4,3]);
        translate([-2,-31,0]) cube([4,62,3]);
        translate([-16,-18,3]) cube([32,2.2,5]);
        translate([-16,15.8,3]) cube([32,2.2,5]);
        // Square rod sockets.
        for (a=[0,120,240])
            rotate([0,0,a]) translate([28,0,0])
                difference() {
                    cylinder(d=10,h=8);
                    translate([-2.25,-2.25,2]) cube([4.5,4.5,6.2]);
                }
        translate([34,-10,0]) rrbox([18,20,3],3);
        mark("C",[0,-27,3],5);
      }
      translate([44,0,-0.1]) hull() {
          translate([0,-4,0]) cylinder(d=4.5,h=3.2);
          translate([0,4,0]) cylinder(d=4.5,h=3.2);
      }
    }
}

module D_louver() {
    difference() {
        union() {
            difference() {
                cylinder(d=70,h=2.2);
                translate([0,0,-0.1]) cylinder(d=38,h=2.4);
            }
            // Integrated spacers face upward; each new louver rests on them.
            for (a=[0,120,240])
                rotate([0,0,a]) translate([28,0,0])
                    cylinder(d=9,h=9);
            mark("D",[0,-27,2.2],4);
        }
        for (a=[0,120,240])
            rotate([0,0,a]) translate([28,0,-0.1])
                translate([-2.35,-2.35,0]) cube([4.7,4.7,9.2]);
    }
}

module E_roof() {
    difference() {
        union() {
            cylinder(d=72,h=3);
            translate([0,0,3]) cylinder(d1=68,d2=58,h=5);
            mark("E",[0,-25,8],4);
        }
        for (a=[0,120,240])
            rotate([0,0,a]) translate([28,0,-0.1])
                translate([-2.3,-2.3,0]) cube([4.6,4.6,8.3]);
    }
}

module F_rod() {
    // Printed flat. Three identical rods slide through the D louvers.
    union() {
        translate([2,0,0]) cube([64,4,4]);
        translate([0,0,0]) hull() {
            translate([0.8,0.4,0.4]) cube([1.2,3.2,3.2]);
            translate([2,0,0]) cube([1,4,4]);
        }
        translate([66,0,0]) hull() {
            cube([1,4,4]);
            translate([1,0.4,0.4]) cube([1.2,3.2,3.2]);
        }
        mark("F",[34,2,4],3.2,0.45);
    }
}

module G_sht_sled() {
    difference() {
        union() {
            rrbox([34,31,2],2);
            for (x=[3,29]) translate([x,5,2]) cube([2,21,4]);
            for (y=[5,24]) translate([3,y,2]) cube([28,2,4]);
            mark("G",[17,27.5,2],4);
        }
        translate([12,10,-0.1]) cube([10,11,2.2]);
        translate([14,-0.1,-0.1]) cube([6,7,4]);
    }
}

module H_sky_top() {
    difference() {
        union() {
            rrbox([92,50,4],4);
            translate([-10,15,0]) rrbox([112,20,4],3);
            translate([27,25,4]) difference() {
                cylinder(d=30,h=18);
                translate([0,0,-0.1]) cylinder(d=16,h=18.2);
            }
            translate([66,25,4]) difference() {
                cylinder(d=28,h=31);
                translate([0,0,-0.1]) cylinder(d1=12,d2=9,h=31.2);
            }
            mark("H  SKY TOP",[46,7,4],4.5);
        }
        // Sensor apertures.
        translate([27,25,-0.1]) cylinder(d=16,h=4.2);
        translate([66,25,-0.1]) cylinder(d=10,h=4.2);
        // Friction peg sockets.
        for (x=[7,85], y=[7,43])
            translate([x,y,-0.1])
                cylinder(d=peg_d+peg_clearance_d,h=4.2);
        for (x=[-5,97])
            translate([x,25,-0.1]) hull() {
                translate([0,-4,0]) cylinder(d=4.5,h=4.2);
                translate([0,4,0]) cylinder(d=4.5,h=4.2);
            }
    }
}

module pocket_rails(c=[27,25], s=[22,18]) {
    translate([c[0]-s[0]/2-1.5,c[1]-s[1]/2-1.5,2]) {
        cube([s[0]+3+board_clearance,1.5,3]);
        translate([0,s[1]+1.5+board_clearance,0])
            cube([s[0]+3+board_clearance,1.5,3]);
        cube([1.5,s[1]+3+board_clearance,3]);
        translate([s[0]+1.5+board_clearance,0,0])
            cube([1.5,s[1]+3+board_clearance,3]);
    }
}

module I_sky_tray() {
    union() {
        rrbox([92,50,2],4);
        pocket_rails([27,25],[22,18]);
        pocket_rails([66,25],[20,18]);
        for (x=[7,85], y=[7,43])
            translate([x,y,2]) cylinder(d1=peg_d,d2=peg_d-0.1,h=4);
        mark("I  SENSOR TRAY",[46,7,2],4);
    }
}

module J_rain_raw() {
    difference() {
      union() {
        // Sloped 3 mm tray.
        hull() {
            translate([0,0,0]) cube([64,4,3]);
            translate([0,48,20]) cube([64,4,3]);
        }
        // Side lips.
        for (x=[0,61])
            hull() {
                translate([x,2,2]) cube([3,4,5]);
                translate([x,46,20]) cube([3,4,5]);
            }
        translate([0,0,2]) cube([64,4,7]);
        translate([22,47,18]) difference() {
            cube([20,8,8]);
            translate([5,-0.1,2]) cube([10,8.2,4]);
        }
        if (is_undef($show_part_labels) || $show_part_labels)
            // Side label; this becomes upward-facing after export rotation.
            translate([0,25,11]) rotate([90,0,90])
                linear_extrude(0.6)
                    text("J RAIN",size=5,font="Liberation Sans:style=Bold",
                         halign="center",valign="center");
      }
      for (x=[10,54])
          translate([x,-0.1,5.5]) rotate([-90,0,0])
              cylinder(d=4,h=4.2);
    }
}

module J_rain_mount() {
    // Rotate onto the broad left cheek: stable, support-free print orientation.
    translate([26,0,0]) rotate([0,-90,0]) J_rain_raw();
}

module K_mounting_plate() {
    union() {
        difference() {
            rrbox([140,100,4],6);
            // Two straps pass over the electronics box.
            for (x=[30,98], y=[3,91])
                translate([x,y,-0.1]) rrbox([12,6,4.2],2);
            // Optional vertical-mast strap passages.
            for (y=[28,66])
                translate([3,y,-0.1]) rrbox([6,16,4.2],2);
        }
        // Low guides locate the box; reusable straps through the plate secure it.
        for (y=[8,90.9]) {
            translate([8,y,4]) cube([122,2.5,4]);
        }
        translate([130,8,4]) cube([3,85.4,6]);
        mark("K  MOUNT",[70,50,4],5);
    }
}

module L_fit_coupon() {
    // Four deliberately separate pieces in one STL. Print before the full set.
    // L1/L2 reproduce the lid clearance; L3/L4 reproduce the sky-head pegs.
    difference() {
        rrbox([40,32,8],3);
        translate([2.4,2.4,2]) rrbox([35.2,27.2,6.2],1.5);
    }
    mark("L1",[20,16,2],4);

    translate([45,2,0]) {
        rrbox([35.2-2*lid_clearance_per_side,
               27.2-2*lid_clearance_per_side,4],2);
        mark("L2",[17.3,13.3,4],4);
    }

    translate([45,35,0]) {
        rrbox([18,18,2],2);
        translate([9,9,2]) cylinder(d1=peg_d,d2=peg_d-0.1,h=4);
        mark("L3",[9,3.5,2],3);
    }

    translate([67,35,0]) difference() {
        union() {
            rrbox([18,18,6],2);
            mark("L4",[9,3.5,6],3);
        }
        translate([9,9,-0.1])
            cylinder(d=peg_d+peg_clearance_d,h=6.2);
    }
}

module assembly_preview() {
    color("lightgray") K_mounting_plate();
    color("slategray") translate([10,10,11]) A_box();
    color("white") translate([8,8,60]) B_lid();

    color("white") translate([180,35,0]) C_shield_base();
    for (z=[10:10:60])
        color("gainsboro") translate([180,35,z]) D_louver();
    color("white") translate([180,35,70]) E_roof();

    color("orange") translate([15,125,0]) I_sky_tray();
    color("darkorange") translate([15,125,8]) H_sky_top();
    color("silver") translate([165,115,0]) J_rain_mount();
}

if (part_id==1) A_box();
else if (part_id==2) B_lid();
else if (part_id==3) C_shield_base();
else if (part_id==4) D_louver();
else if (part_id==5) E_roof();
else if (part_id==6) F_rod();
else if (part_id==7) G_sht_sled();
else if (part_id==8) H_sky_top();
else if (part_id==9) I_sky_tray();
else if (part_id==10) J_rain_mount();
else if (part_id==11) K_mounting_plate();
else if (part_id==12) L_fit_coupon();
else assembly_preview();
