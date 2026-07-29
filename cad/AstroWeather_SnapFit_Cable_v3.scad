/*
  SPDX-License-Identifier: AGPL-3.0-or-later
  Copyright (C) 2026 Tarun and contributors

  AstroWeather India - non-cellular snap-fit and cable-management revision 3

  Every exported STL is in its intended support-free print orientation.
  Print part 12 first and tune the parameters below before the full set.

   1 A3_SnapFit_Electronics_Box
   2 B3_Release_Lid
   3 C_Shield_Base (unchanged sensor geometry)
   4 D_Shield_Louver (print 6)
   5 E_Shield_Roof
   6 F_Shield_Rod (print 3)
   7 G_SHT31_Sled
   8 H_Sky_Head_Top
   9 I3_SnapFit_Sky_Tray
  10 J_Rain_Plate_Mount
  11 K3_Cable_Managed_Mounting_Plate
  12 L3_Fit_Kit_6_PIECES_PRINT_FIRST
  13 M3_Cable_Clip (print 6)
  14 N3_Split_Grommet (print 2)
  90 exploded assembly preview
*/

use <AstroWeather_PrintReady_v2.scad>

$fn = 64;
part_id = 90;

wall = 2.4;
lid_clearance_per_side = 0.35;
snap_tab_width = 14;
snap_tab_thickness = 1.8;
snap_tab_height = 11.2;
snap_hook = 0.75;
snap_window_clearance = 0.45;

sky_peg_shaft_d = 3.45;
sky_peg_head_d = 3.90;
sky_socket_d = 3.75; // Matches the H v2 socket.
board_clearance = 0.50;

cable_entry_d = 10.0;
grommet_body_d = 9.55;
grommet_flange_d = 13.0;
grommet_cable_d = 5.4;
cable_clip_d = 5.5;

module v3_mark(txt, p=[0,0,0], size=5, h=0.55) {
    translate(p) linear_extrude(h)
        text(txt,size=size,font="Liberation Sans:style=Bold",
             halign="center",valign="center");
}

module snap_window(y_back=false) {
    translate([60-snap_tab_width/2-snap_window_clearance,
               y_back ? 77.3 : -0.1,
               31.3])
        cube([snap_tab_width+2*snap_window_clearance,
              2.8,
              4.6]);
}

module cable_anchor(x, y=12) {
    translate([x,y,3])
        difference() {
            union() {
                translate([-5,-4,0]) cube([10,8,6]);
                translate([-6,-5,0]) cube([12,10,1.2]);
            }
            translate([0,0,3.4])
                rotate([90,0,0]) cylinder(d=3.6,h=12,center=true);
        }
}

module board_tie_anchor(x,y) {
    translate([x,y,3])
        difference() {
            union() {
                translate([-4,-3,0]) cube([8,6,5]);
                translate([-5,-4,0]) cube([10,8,1]);
            }
            translate([0,0,3])
                rotate([90,0,0]) cylinder(d=3.4,h=9,center=true);
        }
}

module A3_box() {
    difference() {
        union() {
            difference() {
                rrbox([120,80,42],5);
                translate([wall,wall,3])
                    rrbox([120-2*wall,80-2*wall,42],3.2);
                // Two labelled down-facing entries: power and combined sensor loom.
                translate([20,-1,13])
                    rotate([-90,0,0]) cylinder(d=cable_entry_d,h=wall+2);
                translate([100,-1,13])
                    rotate([-90,0,0]) cylinder(d=cable_entry_d,h=wall+2);
                // Vent/drain slots stay on the installed underside.
                for (x=[29:21:92])
                    translate([x,18,-0.1]) rrbox([10,3,4],1);
            }
            // Generic ESP32/perfboard rails.
            for (y=[25,55]) translate([25,y,3]) cube([70,2.4,4]);
            // Low cable-routing fences guide wires away from the lid latches.
            for (x=[31,89]) translate([x,7,3]) cube([2,18,3]);
            cable_anchor(20);
            cable_anchor(100);
            for (x=[30,90], y=[33,48]) board_tie_anchor(x,y);
            v3_mark("A3  SNAP BOX",[60,68,3],5);
            v3_mark("POWER",[20,22,3],3.2);
            v3_mark("SENSORS",[100,22,3],3.2);
            translate([60,0,38.5]) rotate([-90,0,0])
                linear_extrude(0.5)
                    text("PUSH",size=3.2,
                         font="Liberation Sans:style=Bold",
                         halign="center",valign="center");
            translate([60,80,38.5]) rotate([90,0,0])
                linear_extrude(0.5)
                    text("PUSH",size=3.2,
                         font="Liberation Sans:style=Bold",
                         halign="center",valign="center");
        }
        snap_window(false);
        snap_window(true);
    }
}

module snap_tongue(front=true) {
    union() {
        translate([-snap_tab_width/2,0,0])
            cube([snap_tab_width,snap_tab_thickness,snap_tab_height]);
        if (front)
            hull() {
                translate([-snap_tab_width/2,
                           -snap_hook,
                           snap_tab_height-3.2])
                    cube([snap_tab_width,snap_tab_thickness+snap_hook,0.8]);
                translate([-snap_tab_width/2,
                           0,
                           snap_tab_height-0.25])
                    cube([snap_tab_width,snap_tab_thickness,0.25]);
            }
        else
            hull() {
                translate([-snap_tab_width/2,
                           0,
                           snap_tab_height-3.2])
                    cube([snap_tab_width,snap_tab_thickness+snap_hook,0.8]);
                translate([-snap_tab_width/2,
                           0,
                           snap_tab_height-0.25])
                    cube([snap_tab_width,snap_tab_thickness,0.25]);
            }
        // Root gusset remains below the flexing section.
        hull() {
            translate([-snap_tab_width/2,0,0])
                cube([snap_tab_width,snap_tab_thickness,0.5]);
            translate([-snap_tab_width/2+2,0,2.2])
                cube([snap_tab_width-4,snap_tab_thickness,0.5]);
        }
    }
}

module B3_lid() {
    plug_x = 115.2-2*lid_clearance_per_side;
    plug_y = 75.2-2*lid_clearance_per_side;
    ox = (124-plug_x)/2;
    oy = (84-plug_y)/2;
    ring_t = 1.8;

    union() {
        rrbox([124,84,3],5.5);

        // Alignment rails. Central gaps leave each latch tongue free to flex.
        translate([ox,oy,2.8]) {
            cube([ring_t,plug_y,4.2]);
            translate([plug_x-ring_t,0,0]) cube([ring_t,plug_y,4.2]);
            for (x=[0,plug_x-34]) {
                translate([x,0,0]) cube([34,ring_t,4.2]);
                translate([x,plug_y-ring_t,0])
                    cube([34,ring_t,4.2]);
            }
        }

        // Releasable opposing latches: press both A3 windows while lifting B3.
        translate([62,oy,2.8]) snap_tongue(true);
        translate([62,oy+plug_y-snap_tab_thickness,2.8])
            snap_tongue(false);

        v3_mark("B3  RELEASE LID",[62,42,3],5.5);
        v3_mark("PRESS SIDES",[62,18,3],3.2);
    }
}

module sky_snap_peg() {
    union() {
        cylinder(d=sky_peg_shaft_d,h=3.3);
        translate([0,0,3.3])
            cylinder(d1=sky_peg_head_d,d2=sky_peg_shaft_d,h=0.9);
    }
}

module I3_sky_tray() {
    union() {
        rrbox([92,50,2],4);
        pocket_rails([27,25],[22,18]);
        pocket_rails([66,25],[20,18]);
        for (x=[7,85], y=[7,43])
            translate([x,y,1.8]) sky_snap_peg();
        // Cable tie bridge between the two sensor pockets.
        translate([43,19,2])
            difference() {
                cube([6,12,4]);
                translate([-0.1,3,1.4]) cube([6.2,6,2.8]);
            }
        v3_mark("I3  SNAP TRAY",[46,7,2],4);
    }
}

module K3_mounting_plate() {
    difference() {
        union() {
            rrbox([160,100,4],6);
            for (y=[8,90.9]) translate([8,y,4]) cube([122,2.5,4]);
            translate([130,8,4]) cube([3,85.4,6]);
            // Raised cable spine protects the loom from the box edge.
            translate([140,7,4]) cube([3,86,3]);
            translate([157,7,4]) cube([3,86,3]);
            v3_mark("K3  MOUNT",[70,50,4],5);
            v3_mark("CABLE",[150,50,4],3.5);
        }
        // Box-retaining straps.
        for (x=[30,98], y=[3,91])
            translate([x,y,-0.1]) rrbox([12,6,4.2],2);
        // Vertical-mast straps.
        for (y=[28,66])
            translate([3,y,-0.1]) rrbox([6,16,4.2],2);
        // Four cable-tie stations along the added spine.
        for (y=[15,35,55,75]) {
            translate([145,y,-0.1]) rrbox([3.2,10,4.2],1);
            translate([152,y,-0.1]) rrbox([3.2,10,4.2],1);
        }
    }
}

module M3_cable_clip() {
    difference() {
        union() {
            rrbox([22,16,3],2);
            // Two 45-degree flexible jaws print without support.
            hull() {
                translate([3,3,2.8]) cube([3,10,1]);
                translate([7.1,3,10.5]) cube([2,10,1.2]);
            }
            hull() {
                translate([16,3,2.8]) cube([3,10,1]);
                translate([12.9,3,10.5]) cube([2,10,1.2]);
            }
            // Inward lead-in nubs leave a 4.6 mm throat.
            hull() {
                translate([7.1,3,9.7]) cube([1.6,10,1.8]);
                translate([8.3,3,8.7]) cube([0.4,10,0.8]);
            }
            hull() {
                translate([13.3,3,9.7]) cube([1.6,10,1.8]);
                translate([13.3,3,8.7]) cube([0.4,10,0.8]);
            }
            v3_mark("M3",[11,8,3],3);
        }
        // Zip-tie slots in the base.
        for (x=[2.5,16.5])
            translate([x,5,-0.1]) rrbox([3,6,3.2],1);
    }
}

module N3_split_grommet() {
    difference() {
        union() {
            cylinder(d=grommet_flange_d,h=1.0);
            cylinder(d=grommet_body_d,h=4.0);
            translate([0,0,3.0]) cylinder(d=grommet_flange_d,h=1.0);
        }
        translate([0,0,-0.1]) cylinder(d=grommet_cable_d,h=4.2);
        // Radial slit lets the grommet open around an already-wired cable.
        translate([-0.75,0,-0.1]) cube([1.5,grommet_flange_d,4.2]);
    }
}

module latch_wall_coupon() {
    difference() {
        union() {
            rrbox([34,24,3],2);
            translate([0,20,2.8]) cube([34,2.4,15]);
            v3_mark("L1",[17,8,3],3.5);
        }
        translate([17-snap_tab_width/2-snap_window_clearance,
                   19.9,
                   6.1])
            cube([snap_tab_width+2*snap_window_clearance,2.7,4.6]);
    }
}

module latch_tongue_coupon() {
    union() {
        rrbox([34,14,3],2);
        translate([17,9,2.8]) snap_tongue(false);
        v3_mark("L2",[17,4,3],3.5);
    }
}

module peg_coupon() {
    union() {
        rrbox([18,18,2],2);
        translate([9,9,1.8]) sky_snap_peg();
        v3_mark("L3",[9,3.5,2],3);
    }
}

module socket_coupon() {
    difference() {
        union() {
            rrbox([18,18,4],2);
            v3_mark("L4",[9,3.5,4],3);
        }
        translate([9,9,-0.1]) cylinder(d=sky_socket_d,h=4.2);
    }
}

module L3_fit_kit() {
    // Six deliberately separate pieces.
    latch_wall_coupon();
    translate([40,0,0]) latch_tongue_coupon();
    translate([80,0,0]) peg_coupon();
    translate([103,0,0]) socket_coupon();
    translate([80,25,0]) M3_cable_clip();
    translate([118,35,0]) N3_split_grommet();
}

module assembly_preview_v3() {
    color("lightgray") K3_mounting_plate();
    color("slategray") translate([10,10,11]) A3_box();
    // Representative electronics volumes, not board-specific mounting claims.
    color("forestgreen") translate([38,38,16]) cube([52,28,3]);
    color("seagreen") translate([100,36,16]) cube([20,34,3]);
    // Installed orientation, lifted 15 mm above A3 to expose both latch tongues.
    color("white",0.92)
        translate([8,92,71]) rotate([180,0,0]) B3_lid();
    color("deepskyblue")
        for (y=[15,35,55,75]) translate([139,y,11]) M3_cable_clip();

    color("white") translate([205,35,0]) C_shield_base();
    for (z=[10:10:60])
        color("gainsboro") translate([205,35,z]) D_louver();
    color("white") translate([205,35,70]) E_roof();

    color("orange") translate([15,135,0]) I3_sky_tray();
    color("darkorange") translate([15,135,8]) H_sky_top();
    color("silver") translate([180,125,0]) J_rain_mount();
}

module interface_preview_v3() {
    color("slategray") A3_box();
    color("forestgreen") translate([28,26,8]) cube([52,28,3]);
    color("seagreen") translate([87,25,8]) cube([20,34,3]);
    // Lid shown in installed orientation and lifted 15 mm.
    color("white",0.92)
        translate([-2,82,60]) rotate([180,0,0]) B3_lid();

    color("orange") translate([150,0,0]) I3_sky_tray();
    color("darkorange") translate([150,0,20]) H_sky_top();
    color("deepskyblue") translate([150,72,0]) M3_cable_clip();
    color("gold") translate([205,80,0]) N3_split_grommet();
}

if (part_id==1) A3_box();
else if (part_id==2) B3_lid();
else if (part_id==3) C_shield_base();
else if (part_id==4) D_louver();
else if (part_id==5) E_roof();
else if (part_id==6) F_rod();
else if (part_id==7) G_sht_sled();
else if (part_id==8) H_sky_top();
else if (part_id==9) I3_sky_tray();
else if (part_id==10) J_rain_mount();
else if (part_id==11) K3_mounting_plate();
else if (part_id==12) L3_fit_kit();
else if (part_id==13) M3_cable_clip();
else if (part_id==14) N3_split_grommet();
else if (part_id==91) interface_preview_v3();
else assembly_preview_v3();
