# Hardware Documentation

## Introduction

This document summarizes our experience with the NXP car platform, focusing on the mechanical design choices we made, the problems we encountered during qualifications and preparation, and the lessons learned throughout the process. The goal of this documentation is to help future teams avoid the same mistakes by sharing practical insights related to chassis design, wheels, camera placement, electronics layout, and overall vehicle dynamics.

All points described here are based on real competition constraints and testing results, and should be considered as guidance rather than strict rules.

---

## Chassis

The NXP-provided 3D model of the chassis worked well initially. However, after the qualification round, we realized that several changes were necessary.

Although it was allowed to have two wheels off the track, the elevated edges of the track meant that having wheels off-track caused loss of balance and time. To ensure that all wheels remained on track, we made the following modifications:

* We reduced the distance between the rear wheels and the front wheels.
* We increased the steering angle by leaving more space for the wheels to turn.

These changes reduced the turning radius of the car, but they also introduced new issues:

* **Reduced wheelbase:** Decreasing the distance between the front and rear wheels resulted in a smaller wheelbase. In our case, considering that we had a camera support mounted on top, this led to a less stable car, reduced handling quality, and lower average cornering speed.
* **High steering angle effects:** Increasing the steering angle created problems while cornering. Since the car had no braking system, the momentum carried from straight sections caused the front wheels to drag during sharp turns. The wheels were turning aggressively while the car's inertia wanted to continue forward, resulting in front-wheel slipping, increased strain on the steering mechanism, and higher torque demand from the rear wheels.

---

## Wheels

We used the wheels sold with the JGA25 600 RPM motors. These wheels offered decent grip but were not ideal. The wheels we used were older versions (semi-slicks). Newer versions of these wheels have more grooves, which results in reduced grip.

More details and recommendations regarding wheels are provided in the **Recommendations** section.

---

## Pixy2 Camera Support

Before the qualifications, we used a slightly taller version of the NXP camera support. While it worked mechanically, the Pixy2 detected a lot of background noise from the track, leading to false readings.

After the qualifications, we made several changes to the camera support:

* The new code structure required the camera to see both track lines almost all the time (except for a few edge cases). To avoid background noise, the camera had to be mounted significantly higher, which negatively affected the car's stability.
* In the 3D model (GrabCAD link below), we placed the camera above the front wheels so the car could react to the part of the track it was currently on. This allowed for real-time detection and reaction, which explains the adjustable mechanism controlling the camera's position relative to the wheels.
* To achieve the required height, the camera support was split into two coaxial parts joined together using a screw. This was necessary because no available 3D printer could print a support that long in one piece.
* We also added a servo to control the camera angle. The idea was to adjust the camera position on straight sections to increase speed, and to tilt it in corners to anticipate turns earlier.

Later, we discovered that due to the Pixy2 frame rate, data transfer rate, and the car's speed, there was not enough time to make these adjustments effectively. As a result, this idea was abandoned early in the preparation phase.

---

## Mistakes to Avoid

* The car's structure was heavily driven by the code architecture and the desired camera position, while largely disregarding balance, cornering behavior, and basic vehicle physics.
* The PCB we designed after the qualifications was unreliable and frequently failed due to faulty connections and short circuits.
* The electronic layout was not clean or well-organized, which made debugging far more difficult than necessary.

---

## Recommendations

(In addition to those mentioned in the information session presentation)

* Study car mechanical design at a basic level. While deep mechanical engineering knowledge is not required, a solid understanding of vehicle balance, cornering, weight distribution, grip, and overall structure is essential. We recommend watching Formula 1 car analysis videos to better understand the issues we encountered. Relevant books and videos will be linked below.
* Define a target speed for your car by analyzing previous international events and qualification runs. The Romanian qualifications are highly recommended for reference. This target speed will help determine the appropriate motors. If the budget allows, consider purchasing motors and microcontrollers from international suppliers.
* The current steering mechanism design works well. Adding ball bearings to rotating joints is strongly recommended. This type of steering mechanism has been used by many teams that reached the finals, although other designs also exist and are worth exploring.
* **Wheels:** There is a wide variety of options available outside Tunisia (e.g., JSumo). We recommend 3D-printing custom wheels and fitting them with silicone tires. Avoid tires with grooves, as they reduce grip—this car is not driving in wet conditions.
* Consider the electronics layout during the mechanical design phase. Ensure easy access to the battery and maintain a clear, organized cable layout to simplify debugging and maintenance.
* Carefully read and apply the recommendations mentioned in the information session presentation. The points listed above focus specifically on the mechanical design of the car.

---

## Additional Resources

GrabCad link: https://grabcad.com/library/nxp-cup-2025-finals-car-1
