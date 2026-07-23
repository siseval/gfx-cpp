#include "common/animations/space/simulate.h"
#include "common/animations/space/space-demo.h"
#include "common/animations/space/units.h"

namespace demos
{
    using namespace gfx;

    void Simulate::solar_system(SpaceDemo& demo)
    {
        demo.set_camera_pos({ 0, 0 });
        demo.set_camera_size(3.0);

        demo.spawn_body("sun", { 0.0, 0.0 }, { 0.0, 0.0 }, 0.00465, 1.0, true, { 1.0, 1.0, 0.0, 1.0 });

        auto orbital_velocity = [](const double GM, const double radius_AU)
        {
            return std::sqrt(GM / radius_AU);
        };

        struct Planet
        {
            std::string name;
            double radius_AU;
            double mass_solar;
            double radius_AU_body;
            Color4 color;
        };

        Planet planets[] = {
            { "mercury", 0.387, 1.660e-7, 1.45e-5, { 0.5, 0.5, 0.5 } },
            { "venus", 0.723, 2.447e-6, 3.75e-5, { 1.0, 0.8, 0.3 } },
            { "earth", 1.000, 3.003e-6, 3.37e-5, { 0.0, 0.4, 1.0 } },
            { "mars", 1.524, 3.213e-7, 1.79e-5, { 0.8, 0.2, 0.2 } },
            { "jupiter", 5.203, 9.545e-4, 3.78e-4, { 1.0, 0.7, 0.0 } },
            { "saturn", 9.537, 2.857e-4, 3.21e-4, { 1.0, 1.0, 0.5 } },
            { "uranus", 19.191, 4.364e-5, 1.36e-4, { 0.5, 1.0, 1.0 } },
            { "neptune", 30.068, 5.148e-5, 1.33e-4, { 0.0, 0.0, 0.7 } }
        };

        for (const auto& p : planets)
        {
            double v = orbital_velocity(units::G, p.radius_AU);
            demo.spawn_body(
                p.name,
                { p.radius_AU, 0.0 },
                { 0.0, v },
                p.radius_AU_body,
                p.mass_solar,
                false,
                p.color
            );
        }

        struct Moon
        {
            std::string name;
            int planet_index;
            double orbital_radius_km;
            double mass_solar;
            double radius_AU_body;
            Color4 color;
        };

        auto km_to_au = [](const double km)
        {
            return km / 1.496e8;
        };

        Moon moons[] = {
            { "moon", 2, 384400, 3.7e-8, km_to_au(1737.1), { 0.8, 0.8, 0.8, 1.0 } },

            { "phobos", 3, 9376, 1.07e-11, km_to_au(11.3), { 0.6, 0.6, 0.6, 1.0 } },
            { "deimos", 3, 23460, 1.48e-12, km_to_au(6.2), { 0.7, 0.7, 0.7, 1.0 } },

            { "io", 4, 421700, 4.65e-8, km_to_au(1821.6), { 1.0, 0.8, 0.2, 1.0 } },
            { "europa", 4, 671034, 2.53e-8, km_to_au(1560.8), { 0.7, 0.7, 1.0, 1.0 } },
            { "ganymede", 4, 1070412, 7.8e-8, km_to_au(2634.1), { 0.8, 0.8, 0.6, 1.0 } },
            { "callisto", 4, 1882709, 5.66e-8, km_to_au(2410.3), { 0.5, 0.5, 0.5, 1.0 } },

            { "titan", 5, 1221870, 1.3452e-8, km_to_au(2574.7), { 1.0, 0.7, 0.4, 1.0 } },

            { "miranda", 6, 129390, 6.59e-11, km_to_au(235.8), { 0.7, 0.7, 0.9, 1.0 } },

            { "triton", 7, 354759, 2.14e-8, km_to_au(1353.4), { 0.6, 0.6, 0.8, 1.0 } }
        };

        for (const auto& m : moons)
        {
            const auto& planet = planets[m.planet_index];
            const double r_moon_AU = km_to_au(m.orbital_radius_km);
            const double v_moon = orbital_velocity(units::G * planet.mass_solar, r_moon_AU);

            const Vec2d pos = { planet.radius_AU + r_moon_AU, 0.0 };
            const double v_planet = orbital_velocity(units::G, planet.radius_AU);
            const Vec2d vel = { 0.0, v_planet + v_moon };

            demo.spawn_body(
                m.name,
                pos,
                vel,
                m.radius_AU_body,
                m.mass_solar,
                false,
                m.color
            );
        }
    }

    void Simulate::three_body_problem(SpaceDemo& demo)
    {
        demo.set_camera_pos({ 0, 0 });
        demo.set_camera_size(4.0e11);

        demo.spawn_body(
            "body 1",
            { -1.0e11, 0.0 },
            { 0.0, -1.2e4 },
            1.0e10,
            2.0e30,
            true,
            { 1.0, 0.5, 0.5, 1.0 }
        );

        demo.spawn_body(
            "body 2",
            { 1.0e11, 0.0 },
            { 0.0, 1.2e4 },
            1.0e10,
            2.0e30,
            true,
            { 0.5, 1.0, 0.5, 1.0 }
        );

        demo.spawn_body(
            "body 3",
            { 0.0, 1.5e11 },
            { -1.5e4, 0.0 },
            1.0e10,
            2.0e30,
            true,
            { 0.5, 0.5, 1.0, 1.0 }
        );
    }

    void Simulate::chaos(SpaceDemo& demo)
    {
        demo.set_camera_pos({ 0, 0 });
        demo.set_camera_size(1.0e10);

        demo.spawn_body(
            "core",
            { 0.0, 0.0 },
            { 0.0, 0.0 },
            1.0e7,
            1.0e26,
            false,
            { 1.0, 0.6, 0.0, 1.0 }
        );

        demo.spawn_body("body_1", { 5.0e7, 0.0 }, { 0.0, 1.2e4 }, 2.0e6, 1.0e22, { 0.8, 0.2, 0.2, 1.0 });
        demo.spawn_body("body_2", { -6.0e7, 2.0e7 }, { -1.0e4, -0.8e4 }, 1.8e6, 8.0e21, { 0.2, 0.8, 0.3, 1.0 });
        demo.spawn_body("body_3", { 0.0, -8.0e7 }, { 1.4e4, 0.0 }, 1.5e6, 9.0e21, { 0.4, 0.4, 1.0, 1.0 });
        demo.spawn_body("body_4", { 3.0e7, 4.0e7 }, { -0.7e4, 1.1e4 }, 1.0e6, 6.0e21, { 0.9, 0.9, 0.2, 1.0 });
        demo.spawn_body("body_5", { -2.5e7, -3.5e7 }, { 0.9e4, -0.6e4 }, 1.2e6, 7.0e21, { 0.6, 0.1, 0.9, 1.0 });
        demo.spawn_body("body_6", { 1.0e7, -1.5e7 }, { 1.2e4, 0.8e4 }, 1.4e6, 7.5e21, { 0.2, 0.7, 1.0, 1.0 });
        demo.spawn_body("body_7", { -1.2e7, 1.8e7 }, { -1.0e4, 0.5e4 }, 1.1e6, 5.5e21, { 1.0, 0.3, 0.6, 1.0 });
        demo.spawn_body("body_8", { 2.2e7, -2.0e7 }, { 0.5e4, -1.3e4 }, 1.3e6, 6.2e21, { 0.3, 1.0, 0.8, 1.0 });
        demo.spawn_body("body_9", { -2.0e7, 2.5e7 }, { 0.7e4, 1.0e4 }, 1.0e6, 5.8e21, { 0.5, 0.5, 0.5, 1.0 });
        demo.spawn_body("body_10", { 4.0e7, -1.0e7 }, { -1.1e4, 1.0e4 }, 1.5e6, 8.5e21, { 1.0, 0.8, 0.6, 1.0 });

        demo.spawn_body("frag_1", { 3.0e6, 0.0 }, { 0.0, 1.8e4 }, 5.0e5, 5.0e20, { 0.7, 0.7, 1.0, 1.0 });
        demo.spawn_body("frag_2", { -4.0e6, 1.0e6 }, { -1.5e4, -1.2e4 }, 4.0e5, 4.5e20, { 0.6, 0.4, 0.9, 1.0 });
        demo.spawn_body("frag_3", { 0.0, 5.0e6 }, { 1.7e4, 0.0 }, 3.5e5, 3.8e20, { 0.3, 0.6, 0.9, 1.0 });
        demo.spawn_body("frag_4", { -2.0e6, -3.0e6 }, { 1.2e4, -1.1e4 }, 3.0e5, 3.0e20, { 0.8, 0.4, 0.4, 1.0 });

        demo.spawn_body("flyby", { -1.0e8, -1.0e8 }, { 5.0e4, 6.0e4 }, 2.0e6, 1.0e22, { 0.9, 1.0, 0.9, 1.0 });
    }

    void Simulate::binary_system(SpaceDemo& demo)
    {
        demo.set_camera_pos({ 0, 0 });
        demo.set_camera_size(10.0);

        constexpr double mass1 = 1.0;
        constexpr double mass2 = 0.8;
        constexpr double separation = 1.0;

        constexpr double r1 = mass2 / (mass1 + mass2) * separation;
        double r2 = mass1 / (mass1 + mass2) * separation;

        const double v_total = sqrt(units::G * (mass1 + mass2) / separation);

        demo.spawn_body(
            "star_a",
            { -r1, 0.0 },
            { 0.0, v_total * (mass2 / (mass1 + mass2)) },
            0.00465,
            mass1,
            false,
            { 1.0, 0.9, 0.6, 1.0 }
        );

        demo.spawn_body(
            "star_b",
            { r2, 0.0 },
            { 0.0, -v_total * (mass1 / (mass1 + mass2)) },
            0.0040,
            mass2,
            false,
            { 0.8, 0.9, 1.0, 1.0 }
        );

        auto orbital_velocity = [](const double GM, const double radius_AU)
        {
            return sqrt(GM / radius_AU);
        };

        constexpr double system_mass = mass1 + mass2;

        struct Planet
        {
            std::string name;
            double radius_AU;
            double mass_solar;
            double radius_AU_body;
            Color4 color;
        };

        Planet planets[] = {
            { "circumbinary_1", 2.5, 1e-5, 2.5e-5, { 0.6, 0.8, 1.0, 1.0 } },
            { "circumbinary_2", 4.0, 2e-5, 3.0e-5, { 1.0, 0.6, 0.2, 1.0 } },
            { "circumbinary_giant", 6.5, 9e-4, 4.0e-4, { 1.0, 0.7, 0.3, 1.0 } }
        };

        for (const auto& p : planets)
        {
            double v = orbital_velocity(units::G * system_mass, p.radius_AU);
            demo.spawn_body(
                p.name,
                { p.radius_AU, 0.0 },
                { 0.0, v },
                p.radius_AU_body,
                p.mass_solar,
                false,
                p.color
            );
        }

        constexpr double star_b_orbit_radius = 0.2;
        const double star_b_orbit_velocity = orbital_velocity(units::G * mass2, star_b_orbit_radius);

        const Vec2d star_b_pos = { r2, 0.0 };
        const Vec2d star_b_vel = { 0.0, -v_total * (mass1 / (mass1 + mass2)) };

        const Vec2d moon_pos = { star_b_pos.x + star_b_orbit_radius, star_b_pos.y };
        const Vec2d moon_vel = { star_b_vel.x, star_b_vel.y + star_b_orbit_velocity };

        demo.spawn_body(
            "captured_moon",
            moon_pos,
            moon_vel,
            1.5e-5,
            5e-7,
            false,
            { 0.7, 0.7, 0.7, 1.0 }
        );
    }
}
