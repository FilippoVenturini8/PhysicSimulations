# Physics Simulations
This project contains simulations of different physical behaviors implemented in four different scenes.

## Projectile Simulation

<p align="center">
  <img src="https://github.com/FilippoVenturini8/PhysicSimulations/assets/73821401/9693596a-0a3c-4a07-9a73-8f8145a37f2e" width="50%">
</p>

In this simulation, a projectile trajectory is simulated and compared to the analytic one, using different types of integrators such as `Euler`, `Symplectic Euler`, `Verlet`, and `RK2`.

## Particle System

<p align="center">
  <img src="https://github.com/FilippoVenturini8/PhysicSimulations/assets/73821401/5ab03e89-d91b-4392-9863-10506779d0d6" width="50%">
</p>

In this simulation, a `particle system` is implemented to handle an arbitrary number of particles emitted by a fountain. Different types of `collisions` are handled, with the floor, a cube, and a movable sphere. Each particle is subjected to the `gravitational force` and, if activated, to the gravitational force of a `black hole`.

## Cloth Simulation

<p align="center">
  <img src="https://github.com/FilippoVenturini8/PhysicSimulations/assets/73821401/de506539-e966-4df8-a58d-f70575dcdbc3" width="50%">
</p>

In this part, cloth behavior is simulated by adding spring forces between particles following `Provot's Layout`.

## Fluid Simulation

<p align="center">
  <img src="https://github.com/FilippoVenturini8/PhysicSimulations/assets/73821401/8f0eab41-f2f1-45dc-aad7-2f2cbea0a596" width="50%">
</p>

In this last part, a fluid simulation is realized by implementing the `Navier-Stokes` equations and following the `SPH Loop`.
