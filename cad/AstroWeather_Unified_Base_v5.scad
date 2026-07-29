/*
  SPDX-License-Identifier: AGPL-3.0-or-later
  Copyright (C) 2026 Tarun and contributors

  AstroWeather India - non-cellular unified-base revision 5

  Clean, unmarked, tool-less parts. K5 is a single 210 x 200 mm backplane for
  the electronics box, radiation shield, sky head, rain cradle and cable loom.

   1 A5 electronics box
   2 B5 releasable lid
   3 C5 shield base
   4 D5 side-snap louver (print 6)
   5 E5 shield roof
   6 F5 detent rod (print 3)
   7 G5 SHT31 sled
   8 H5 sky head
   9 I5 sky tray
  10 J5 rain cradle
  11 K5 single unified base plate
  12 L5 fit kit - 12 pieces - print first
  13 M5 cable clip (print 6)
  14 N5 split grommet (print 2)
  15 O5 electronics carrier
  90 assembled preview
  91 base interface preview
*/

use <AstroWeather_Toolless_Snap_v4.scad>
use <AstroWeather_SnapFit_Cable_v3.scad>
use <AstroWeather_PrintReady_v2.scad>

$fn = 64;
part_id = 90;
$show_part_labels = false;

base_x = 210;
base_y = 200;
base_t = 4;
edge_capture_clearance = 0.25;
edge_hook_overlap = 0.40;

box_origin = [5,5];
shield_center = [170,45];
sky_origin = [5,112];
rain_origin = [140,112];

module low_edge_clip_y(x,y,back=false,part_t=2) {
    arm_h = part_t + edge_capture_clearance + 1.35;
    translate([x,y,base_t])
        union() {
            cube([10,1.8,arm_h]);
            hull() {
                translate([0,back ? -edge_hook_overlap : 0,arm_h-1.0])
                    cube([10,1.8+edge_hook_overlap,0.8]);
                translate([0,0,arm_h])
                    cube([10,1.8,0.35]);
            }
            hull() {
                translate([2,back ? 1.0 : -0.4,0])
                    cube([6,1.2,0.8]);
                translate([3,back ? 0.6 : 0,1.8])
                    cube([4,1.2,0.8]);
            }
        }
}

module ring_edge_clip(a=0) {
    arm_h = 4.35;
    translate(shield_center)
        rotate([0,0,a])
            translate([36,-5,base_t])
                union() {
                    cube([1.8,10,arm_h]);
                    hull() {
                        translate([-edge_hook_overlap,0,arm_h-1.1])
                            cube([1.8+edge_hook_overlap,10,0.8]);
                        translate([0,0,arm_h])
                            cube([1.8,10,0.35]);
                    }
                    hull() {
                        cube([1.8,10,0.8]);
                        translate([-1.4,2,1.8]) cube([3.2,6,0.8]);
                    }
                }
}

module rain_side_clip(x,y,side=1,top_z=12) {
    // side=1 hooks toward +X; side=-1 hooks toward -X.
    translate([x,y,base_t])
        union() {
            translate([side<0 ? -1.8 : 0,0,0])
                cube([1.8,8,top_z-base_t+0.35]);
            hull() {
                translate([side<0 ? -1.8-edge_hook_overlap : 0,
                           0,top_z-base_t-0.8])
                    cube([1.8+edge_hook_overlap,8,0.8]);
                translate([side<0 ? -1.8 : 0,0,top_z-base_t+0.1])
                    cube([1.8,8,0.25]);
            }
            hull() {
                translate([side<0 ? -1.8 : 0,1,0])
                    cube([1.8,6,0.8]);
                translate([side<0 ? -3.2 : 0,2,3])
                    cube([3.2,4,0.8]);
            }
        }
}

module rain_support_rail(x) {
    hull() {
        translate([x,rain_origin[1],base_t]) cube([3,5,1.6]);
        translate([x,rain_origin[1]+48,base_t+20]) cube([3,5,1.6]);
    }
}

module cable_bridge(x,y) {
    translate([x,y,base_t])
        difference() {
            union() {
                cube([12,5,3]);
                translate([-1,-1,0]) cube([14,7,0.8]);
            }
            translate([3,-0.1,1]) cube([6,5.2,2.2]);
        }
}

module K5_unified_base() {
    difference() {
        union() {
            rrbox([base_x,base_y,base_t],7);

            // A5 box dock: A5 sits at box_origin and releases from four tabs.
            plate_clip(40,2.8,false);
            plate_clip(90,2.8,false);
            plate_clip(40,85.2,true);
            plate_clip(90,85.2,true);

            // C5 shield nest: four fingers retain the 70 mm base ring.
            for (a=[45,135,225,315]) ring_edge_clip(a);

            // I5/H5 sky-head nest captures the 92 x 50 mm tray.
            low_edge_clip_y(25,109.2,false,2);
            low_edge_clip_y(67,109.2,false,2);
            low_edge_clip_y(25,162.8,true,2);
            low_edge_clip_y(67,162.8,true,2);

            // J5 rain cradle rests at its functional slope on two rails.
            rain_support_rail(146);
            rain_support_rail(195);
            rain_side_clip(138.2,118,1,13.4);
            rain_side_clip(205.8,118,-1,13.4);
            rain_side_clip(138.2,148,1,25.3);
            rain_side_clip(205.8,148,-1,25.3);

            // Integrated loom lane and six snap-over bridges.
            translate([4,181,base_t]) cube([202,2,3]);
            translate([4,196,base_t]) cube([202,2,3]);
            for (x=[12,48,84,120,156,192]) cable_bridge(x,186);
        }

        // Mast/wall straps and drainage openings.
        for (x=[12,188],y=[18,88,158])
            translate([x,y,-0.1]) rrbox([10,22,base_t+0.2],2);
        for (x=[28:26:184])
            translate([x,188,-0.1]) rrbox([12,4,base_t+0.2],1.5);
        // Hand access beside each release group.
        for (x=[44,94])
            translate([x,91,-0.1]) rrbox([12,6,base_t+0.2],2);
        translate([130,25,-0.1]) rrbox([8,40,base_t+0.2],2);
        translate([106,122,-0.1]) rrbox([8,28,base_t+0.2],2);
    }
}

module base_clip_coupon() {
    union() {
        rrbox([30,18,base_t],2);
        low_edge_clip_y(10,4,false,3);
    }
}

module captured_edge_coupon() {
    // Exact 3 mm edge used to approve the K5 service clips.
    rrbox([26,12,3],1.5);
}

module L5_fit_kit() {
    L4_fit_kit();
    translate([0,72,0]) base_clip_coupon();
    translate([36,74,0]) captured_edge_coupon();
}

module rain_installed() {
    // Inverse of J4's print rotation: returns the rain surface to its working
    // upward slope and places its low edge on K5.
    translate([rain_origin[0],rain_origin[1],base_t+26])
        rotate([0,90,0]) J4_rain_mount();
}

module assembly_preview_v5() {
    color("lightgray") K5_unified_base();

    color("slategray")
        translate([box_origin[0],box_origin[1],base_t]) A4_box();
    color("forestgreen")
        translate([box_origin[0]+14,box_origin[1]+17,base_t+9])
            O4_electronics_carrier();
    color("white",0.9)
        translate([box_origin[0]-2,box_origin[1]+82,base_t+60])
            rotate([180,0,0]) B4_lid();

    color("white")
        translate([shield_center[0],shield_center[1],base_t])
            rotate([0,0,90]) C4_shield_base();
    for (a=[0,120,240])
        color("silver")
            translate([shield_center[0],shield_center[1],base_t+8])
                rotate([0,0,a+90]) translate([28,0,0])
                    rotate([0,-90,0]) F4_rod();
    for (z=[12:10:62])
        color("gainsboro")
            translate([shield_center[0],shield_center[1],base_t+z])
                rotate([0,0,90]) D4_louver();
    color("white")
        translate([shield_center[0],shield_center[1],base_t+72])
            rotate([0,0,90]) E4_roof();

    color("orange")
        translate([sky_origin[0],sky_origin[1],base_t]) I4_sky_tray();
    color("darkorange")
        translate([sky_origin[0],sky_origin[1],base_t+9]) H4_sky_top();

    color("silver") rain_installed();
}

module interface_preview_v5() {
    color("lightgray") K5_unified_base();
    color("slategray",0.85)
        translate([box_origin[0],box_origin[1],base_t+12]) A4_box();
    color("white",0.9)
        translate([shield_center[0],shield_center[1],base_t+14])
            rotate([0,0,90]) C4_shield_base();
    color("orange",0.9)
        translate([sky_origin[0],sky_origin[1],base_t+12]) I4_sky_tray();
    color("silver",0.9)
        translate([0,0,10]) rain_installed();
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
else if (part_id==11) K5_unified_base();
else if (part_id==12) L5_fit_kit();
else if (part_id==13) M3_cable_clip();
else if (part_id==14) N3_split_grommet();
else if (part_id==15) O4_electronics_carrier();
else if (part_id==91) interface_preview_v5();
else assembly_preview_v5();
