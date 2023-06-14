# This function will be called when the program exits
define on_exit
    if $_exitcode == 0
        # If the program exited normally, rerun it
        run
    else
        # If the program did not exit normally, print the backtrace
        backtrace
    end
end

# This command tells GDB to run 'on_exit' when the program exits
catch exit
commands
    silent
    on_exit
end

# Start the execution of the program
run
