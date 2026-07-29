/*
  SPDX-License-Identifier: AGPL-3.0-or-later
  Copyright (C) 2026 Tarun and contributors

  AstroWeather India - non-cellular tool-less snap assembly revision 4

  Print part 12 first. All exported parts are oriented support-free.

   1 A4 Tool-less electronics box
   2 B4 Releasable lid
   3 C4 Shield base with click sockets
   4 D4 Side-snap louver (print 6)
   5 E4 Click roof
   6 F4 Detent rod (print 3)
   7 G4 Click SHT31 sled
   8 H4 Sky head top
   9 I4 Sky tray with PCB fingers
  10 J4 Compliant rain cradle
  11 K4 Click mounting plate
  12 L4 Complete fit kit - print first
  13 M4 Cable clip (print 6)
  14 N4 Split grommet (print 2)
  15 O4 Electronics carrier
  90 Exploded assembly preview
  91 Interface preview
*/

use <AstroWeather_SnapFit_Cable_v3.scad>
use <AstroWeather_PrintReady_v2.scad>

$fn = 64;
part_id = 90;
$show_part_labels = false;

rod_nominal = 4.00;
rod_socket = 4.35;
rod_detent = 4.50;
louver_throat = 3.65;
louver_socket = 4.25;
pcb_clearance = 0.55;
pcb_hook_overlap = 0.45;
carrier_peg_shaft = 3.40;
carrier_peg_head = 3.85;
carrier_hole = 3.70;
mount_hook_overlap = 0.40;
rain_board_width = 55.0;
rain_board_length = 40.0;

module v4_mark(txt,p=[0,0,0],size=4,h=0.50) {
    if ($show_part_labels)
        translate(p) linear_extrude(h)
            text(txt,size=size,font="Liberation Sans:style=Bold",
                 halign="center",valign="center");
}

module carrier_snap_stud() {
    union() {
        cylinder(d=carrier_peg_shaft,h=4.2);
        translate([0,0,4.2])
            cylinder(d1=carrier_peg_head,d2=carrier_peg_shaft,h=0.9);
    }
}

module A4_box() {
    union() {
        A3_box();
        // Four carrier studs rise above the original internal rails.
        for (x=[25,95],y=[20,60])
            translate([x,y,3]) carrier_snap_stud();
        // External catches for K4. Press K4's four tabs outward to release.
        for (x=[35,85]) {
            translate([x,-1.1,4]) cube([10,2.2,2.2]);
            translate([x,78.9,4]) cube([10,2.2,2.2]);
        }
        v4_mark("A4 TOOLLESS",[60,59,3],4.2);
    }
}

module B4_lid() {
    B3_lid();
}

module rod_click_socket(a=0) {
    rotate([0,0,a]) translate([28,0,0])
        difference() {
            cylinder(d=10,h=8);
            translate([-rod_socket/2,-rod_socket/2,1.7])
                cube([rod_socket,rod_socket,6.5]);
            // Relief slit lets the socket expand around the rod detent.
            translate([-0.75,0,1.7]) cube([1.5,6,6.5]);
        }
}

module C4_shield_base() {
    difference() {
        union() {
            difference() {
                cylinder(d=70,h=3);
                translate([0,0,-0.1]) cylinder(d=38,h=3.2);
            }
            translate([-31,-2,0]) cube([62,4,3]);
            translate([-2,-31,0]) cube([4,62,3]);
            translate([-16,-18,3]) cube([32,2.2,5]);
            translate([-16,15.8,3]) cube([32,2.2,5]);
            for (a=[0,120,240]) rod_click_socket(a);
            // End stop and click bump for G4.
            translate([14.5,-15.7,3]) cube([2.5,31.4,4.5]);
            translate([12.7,-2,5.2]) cube([2.2,4,1.2]);
            translate([34,-10,0]) rrbox([18,20,3],3);
            v4_mark("C4 CLICK",[0,-27,3],3.8);
        }
        translate([44,0,-0.1]) hull() {
            translate([0,-4,0]) cylinder(d=4.5,h=3.2);
            translate([0,4,0]) cylinder(d=4.5,h=3.2);
        }
    }
}

module radial_snap_cut(a=0) {
    rotate([0,0,a]) {
        translate([28-louver_socket/2,-louver_socket/2,-0.1])
            cube([louver_socket,louver_socket,9.3]);
        // Narrow mouth creates two flexible retaining shoulders.
        translate([28,-louver_throat/2,-0.1])
            cube([8,louver_throat,9.3]);
    }
}

module D4_louver() {
    difference() {
        union() {
            difference() {
                cylinder(d=70,h=2.2);
                translate([0,0,-0.1]) cylinder(d=38,h=2.4);
            }
            for (a=[0,120,240])
                rotate([0,0,a]) translate([28,0,0])
                    cylinder(d=9,h=9);
            v4_mark("D4 SNAP",[0,-27,2.2],3.4);
        }
        for (a=[0,120,240]) radial_snap_cut(a);
    }
}

module E4_roof() {
    difference() {
        union() {
            cylinder(d=72,h=3);
            translate([0,0,3]) cylinder(d1=68,d2=58,h=5);
            v4_mark("E4 CLICK",[0,-25,8],3.5);
        }
        for (a=[0,120,240])
            rotate([0,0,a]) translate([28,0,-0.1]) {
                translate([-rod_socket/2,-rod_socket/2,0])
                    cube([rod_socket,rod_socket,8.3]);
                translate([-0.75,0,0]) cube([1.5,6,8.3]);
            }
    }
}

module F4_rod() {
    // Print flat. The 0.25 mm side beads click into C4/E4 relief sockets.
    union() {
        translate([2,0.25,0]) cube([64,rod_nominal,rod_nominal+0.25]);
        for (x=[1.8,65.8]) {
            translate([x,0.25,0.25]) cube([0.8,rod_nominal,rod_nominal]);
            translate([x,0,1.0]) cube([0.8,rod_detent,2.5]);
        }
        translate([0,0.65,0.65]) hull() {
            cube([1.8,3.2,3.2]);
            translate([2,0,0]) cube([0.8,3.2,3.2]);
        }
        translate([66,0.65,0.65]) hull() {
            cube([0.8,3.2,3.2]);
            translate([1.6,0.4,0.4]) cube([0.8,2.4,2.4]);
        }
        v4_mark("F4",[34,2.25,4.25],3,0.4);
    }
}

module G4_sht_sled() {
    union() {
        G_sht_sled();
        // Flexible end tongue clicks behind the C4 centre bump.
        translate([29.5,13.5,2])
            difference() {
                union() {
                    cube([4.5,4,1.6]);
                    translate([2.5,0,1.2]) cube([2,4,1.2]);
                }
                translate([-0.1,0.8,-0.1]) cube([2.8,2.4,2.2]);
            }
        v4_mark("G4",[5,15.5,1.7],2.4);
    }
}

module pcb_finger(x,y,turn=0) {
    translate([x,y,2]) rotate([0,0,turn])
        union() {
            cube([1.6,5.5,4.8]);
            hull() {
                translate([0,3.8,3.4]) cube([1.6,1.7,1.2]);
                translate([pcb_hook_overlap,4.8,4.6])
                    cube([1.6,0.7,0.4]);
            }
        }
}

module I4_sky_tray() {
    union() {
        I3_sky_tray();
        // Releasable fingers retain the nominal 22x18 and 20x18 modules.
        pcb_finger(14.2,14.0,0);
        pcb_finger(39.8,36.0,180);
        pcb_finger(54.2,14.0,0);
        pcb_finger(77.8,36.0,180);
        v4_mark("I4 PCB CLICK",[46,43,2],3.2);
    }
}

module H4_sky_top() {
    H_sky_top();
}

module J4_rain_raw() {
    difference() {
        union() {
            J_rain_raw();
            // Four tapered compliant fingers accept the common 55 x 40 mm plate.
            for (x=[3.5,60.5],y=[7,43])
                translate([x,y,4+y*0.38])
                    hull() {
                        cube([1.8,5,5]);
                        translate([x<10 ? 1.1 : -1.1,0,4.2])
                            cube([1.8,5,0.8]);
                    }
            translate([22,5,4]) cube([20,2,2]);
            v4_mark("J4 RAIN CLICK",[32,27,14],3.4);
        }
        // Finger-root slots allow modest clone-board width variation.
        for (x=[3.2,59.2],y=[8,44])
            translate([x,y,3+y*0.38]) cube([2.6,2.2,4]);
    }
}

module J4_rain_mount() {
    translate([26,0,0]) rotate([0,-90,0]) J4_rain_raw();
}

module plate_clip(x,y,back=false) {
    translate([x,y,4])
        union() {
            cube([10,1.8,7.4]);
            hull() {
                translate([0,back ? -mount_hook_overlap : 0,6.4])
                    cube([10,1.8+mount_hook_overlap,1]);
                translate([0,0,7.4]) cube([10,1.8,0.4]);
            }
            translate([2,back ? 1.8 : -1.2,1.5])
                cube([6,1.2,2.8]);
        }
}

module K4_mounting_plate() {
    union() {
        K3_mounting_plate();
        // Box occupies x=10..130, y=10..90. Four tabs click onto A4 catches.
        plate_clip(45,7.8,false);
        plate_clip(95,7.8,false);
        plate_clip(45,90.2,true);
        plate_clip(95,90.2,true);
        v4_mark("K4 CLICK MOUNT",[70,62,4],4);
    }
}

module board_corner(x,y,sx=1,sy=1) {
    translate([x+(sx<0 ? -2 : 0),y+(sy<0 ? -2 : 0),2])
        cube([2,2,4.5]);
    translate([x+(sx<0 ? -2-pcb_hook_overlap : 0),
               y+(sy<0 ? -2-pcb_hook_overlap : 0),5.5])
        cube([2+pcb_hook_overlap,2+pcb_hook_overlap,1]);
}

module O4_electronics_carrier() {
    difference() {
        union() {
            rrbox([92,46,2],3);
            // ESP32 nominal 52 x 28 mm.
            board_corner(8,9,1,1);
            board_corner(62,9,-1,1);
            board_corner(8,39,1,-1);
            board_corner(62,39,-1,-1);
            // Auxiliary/perfboard nominal 20 x 34 mm.
            board_corner(67,6,1,1);
            board_corner(89,6,-1,1);
            board_corner(67,42,1,-1);
            board_corner(89,42,-1,-1);
            v4_mark("O4 CARRIER",[46,4,1.7],2.8);
        }
        for (x=[11,81],y=[3,43]) {
            translate([x,y,-0.1]) cylinder(d=carrier_hole,h=2.2);
            // Compliance slots reduce force around each hole.
            translate([x-0.55,y<20 ? y : y-5,-0.1])
                cube([1.1,5,2.2]);
        }
        translate([31,18,-0.1]) cube([10,10,2.2]);
        translate([72,18,-0.1]) cube([10,12,2.2]);
    }
}

module rod_coupon() {
    union() {
        rrbox([18,15,3],2);
        translate([8,7,3]) cube([8,rod_nominal,rod_nominal]);
        translate([14,6.75,3.75]) cube([0.8,rod_detent,2.5]);
        v4_mark("R1",[5,5,3],2.8);
    }
}

module louver_coupon() {
    difference() {
        union() {
            rrbox([20,15,7],2);
            v4_mark("R2",[5,5,7],2.8);
        }
        translate([12-louver_socket/2,7.5-louver_socket/2,-0.1])
            cube([louver_socket,louver_socket,7.2]);
        translate([12,7.5-louver_throat/2,-0.1])
            cube([8,louver_throat,7.2]);
    }
}

module carrier_peg_coupon() {
    union() {
        rrbox([18,18,2],2);
        translate([9,9,1.8]) carrier_snap_stud();
        v4_mark("P1",[5,4,2],2.8);
    }
}

module carrier_hole_coupon() {
    difference() {
        union() {
            rrbox([18,18,2],2);
            v4_mark("P2",[5,4,2],2.8);
        }
        translate([9,9,-0.1]) cylinder(d=carrier_hole,h=2.2);
        translate([8.45,9,-0.1]) cube([1.1,6,2.2]);
    }
}

module L4_fit_kit() {
    // V3's six pieces plus four exact v4 interface coupons = ten pieces.
    L3_fit_kit();
    translate([0,48,0]) rod_coupon();
    translate([23,48,0]) louver_coupon();
    translate([50,48,0]) carrier_peg_coupon();
    translate([73,48,0]) carrier_hole_coupon();
}

module assembly_preview_v4() {
    color("lightgray") K4_mounting_plate();
    color("slategray") translate([10,10,4]) A4_box();
    color("forestgreen") translate([24,27,17]) O4_electronics_carrier();
    color("white",0.9) translate([8,92,68]) rotate([180,0,0]) B4_lid();

    for (a=[0,120,240])
        color("silver") translate([205,35,8])
            rotate([0,0,a]) translate([28,0,0])
                rotate([0,-90,0]) F4_rod();
    color("white") translate([205,35,0]) C4_shield_base();
    for (z=[12:10:62])
        color("gainsboro") translate([205,35,z]) D4_louver();
    color("white") translate([205,35,72]) E4_roof();

    color("orange") translate([15,140,0]) I4_sky_tray();
    color("darkorange") translate([15,140,9]) H4_sky_top();
    color("silver") translate([180,130,0]) J4_rain_mount();
}

module interface_preview_v4() {
    color("slategray") A4_box();
    color("forestgreen") translate([14,17,13]) O4_electronics_carrier();
    color("white",0.9) translate([-2,82,60]) rotate([180,0,0]) B4_lid();
    color("white") translate([155,5,0]) C4_shield_base();
    color("gainsboro") translate([155,5,15]) D4_louver();
    color("silver") translate([182,5,9]) rotate([0,-90,0]) F4_rod();
    color("orange") translate([145,100,0]) I4_sky_tray();
    color("darkorange") translate([145,100,18]) H4_sky_top();
}

if (part_id==1) A4_box();
else if (part_id==2) B4_lid();
else if (part_id==3) C4_shield_base();
else if (part_id==4) D4_louver();
else if (part_id==5) E4_roof();
else if (part_id==6) F4_rod();
else if (part_id==7) G4_sht_sled();
else if (part_id==8) H4_sky_top();
else if (part_id==9) I4_sky_tray();
else if (part_id==10) J4_rain_mount();
else if (part_id==11) K4_mounting_plate();
else if (part_id==12) L4_fit_kit();
else if (part_id==13) M3_cable_clip();
else if (part_id==14) N3_split_grommet();
else if (part_id==15) O4_electronics_carrier();
else if (part_id==91) interface_preview_v4();
else assembly_preview_v4();
