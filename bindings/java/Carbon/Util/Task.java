package Carbon.Util;

public class Task<T extends Object> {
    public Task(TaskRunner<T> run)
    {
        _run = run;
        _thread = new java.lang.Thread(run);
        _thread.start();
    }
    
    public T get() {
        if(_thread.isAlive())
        {
            try {
                _thread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        
        return _run.result();
    }

    private java.lang.Thread _thread;
    private TaskRunner<T> _run;
}
