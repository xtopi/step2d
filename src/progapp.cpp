#include "progapp.h"
#include "step.h"
#include "population.h"
#include "precomp.h"

int window_x_size = 1000;
int window_y_size = 777;

// The start of the Application
int TApplication::start(const std::vector<std::string> &args)
{
    // Set the window
    DisplayWindowDescription desc;
    desc.set_title("Step simulation");
    desc.set_size(Size(window_x_size, window_y_size), true);
    desc.set_allow_resize(false);

    DisplayWindow window(desc);

    // Connect the Window close event
    Slot slot_quit = window.sig_window_close().connect(this, &TApplication::on_window_close);

    // Connect a keyboard handler to on_key_up()
    Slot slot_input_up = (window.get_ic().get_keyboard()).sig_key_up().connect(this, &TApplication::on_input_up);

    // Create the canvas
    Canvas canvas(window);

    // create environment (world with floor)
    using NStep::TStep;
    TStep::EKind kind = TStep::Block;
    if (args.size() > 1)
        kind = TStep::EKind(atoi(args[1].c_str()));
    TStep step(kind);
    step.Initialize();

    /*
    if (args.size() > 1) {
        string s = NStep::GetFile(args[1]);
        Dlog << "Got file of size: " << s.size() << " -- " << s << endl;
        step.Load(s);
    }
    */

    NStep::TPopulation population(NStep::TDefs::PopSize, NStep::TDefs::SelSize);

    /*
    size_t winner = population.GetWinner();
    // Dlog << "winner is " << winner << endl;
    if (args.size() > 1 && args[1] == "winner") {
        step.SetCreature(population.Get(winner));
    }
    */

    static const size_t stepLimit = 5 * 60 * 5;
    // unsigned int last_time = System::get_time();

    Dlog << "key assignment:\n"
         << "=======================================\n"
         << "ESC - quit\n"
         << "c   - hide current experiment\n"
         << "b   - hide current generation\n"
         << "v   - hide / show\n"
         << "r   - show leaders (restart generation)\n"
         << "z   - decrease scale\n"
         << "x   - increase scale\n" << endl;
    // Run until someone presses escape
    while (!quit) {
        next = false;
        step.SetCreature(population.GetNextCreature());
        step.StartExperiment();

        if (viewnextcycle && population.IsNextCycle()) {
            viewnextcycle = false;
            viewnext = false;
            view = true;
        }


        PhysicsDebugDraw debug_draw(step.GetWorld());
        debug_draw.set_flags(f_shape);
        // debug_draw.set_flags(f_shape|f_joint | f_center_of_mass);
        size_t nstep = 0;

        // Run until someone presses space
        while (!(next || quit))
        {
            /*
            unsigned int current_time = System::get_time();
            float time_delta_ms = static_cast<float> (current_time - last_time);
            (void)time_delta_ms;
            last_time = current_time;
            */

            if (view) {
                canvas.clear();
                debug_draw.draw(canvas);
                canvas.draw_line(nstep, 700, nstep, 700 - step.GetProgress(), Colorf(0.8f, 0.8f, 0.8f, 1.0f));
                canvas.flush();
                window.flip(1);

                // System::sleep(10);
            }
            // This call processes user input and other events
            KeepAlive::process(0);

            step.StepExperiment();

            ++nstep;
            if (nstep > stepLimit)
                break;
        }
        population.SaveCreature();
        population.SetScore(step.GetProgress());

        if (viewnext) {
            viewnext = false;
            view = true;
        }
        if (restart) {
            population.Restart();
            restart = false;
            Dlog << "restarted" << endl;
        }

        if (zoomOut) {
            step.SetScale(step.GetScale() / 2);
            zoomOut = false;
        }
        if (zoomIn) {
            step.SetScale(step.GetScale() * 2);
            zoomIn = false;
        }

    }

    return 0;
}

// A key was pressed
void TApplication::on_input_up(const InputEvent &key)
{
    if(key.id == keycode_escape) {
        quit = true;
    }
    else if(key.id == clan::keycode_space) {
        next = true;
    }
    else if(key.id == clan::keycode_v) {
        view = !view;
        // Dlog << "view = " << view << endl;
    }
    else if(key.id == clan::keycode_c) {
        view = false;
        viewnext = true;
    }
    else if(key.id == clan::keycode_b) {
        view = false;
        viewnext = false;
        viewnextcycle = true;
    }
    else if (key.id == keycode_r) {
        restart = true;
        Dlog << "restart" << endl;
    }
    else if (key.id == keycode_z) {
        zoomOut = true;
        Dlog << "zoom out" << endl;
    }
    else if (key.id == keycode_x) {
        zoomIn = true;
        Dlog << "zoom in" << endl;
    }
}

// The window was closed
void TApplication::on_window_close()
{
    quit = true;
}
