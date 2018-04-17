package Carbon.Util;

public interface TaskRunner<T>  extends java.lang.Runnable {
    @Override
    void run();
    
    T result();
}
