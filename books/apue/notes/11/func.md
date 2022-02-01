<h1 id=file_func>
    第11章 线程
</h1>

[章节目录](../../README.md#title_ch11 "返回章节目录")
[笔记](notes.md "进入笔记")

---

<table>
    <tr>
        <th>序号</th><th>函数原型</th><th>头文件</th><th>说明</th>
    </tr>
    <tr>
        <td>1</td>
        <td>int pthread_equal(pthread_t tid1, pthread_t tid2);</td>
        <td>pthread.h</td>
        <td>比较两个线程ID是否相等</td>
    </tr>
    <tr>
        <td>2</td>
        <td>pthread_t pthread_self(void);</td>
        <td>pthread.h</td>
        <td>获取调用线程的线程ID。</td>
    </tr>
    <tr>
        <td>3</td>
        <td>int pthread_create(pthread_t *tidp,<br>
            const pthred_attr_t *attr,<br>
            void *(*start_rtn)(void*),<br>
            void *arg);</td>
        <td>pthread.h</td>
        <td>创建一个新线程。</td>
    </tr>
    <tr>
        <td>4</td>
        <td>int pthread_exit(void *rval_ptr);</td>
        <td>pthread.h</td>
        <td>退出线程。</td>
    </tr>
    <tr>
        <td>5</td>
        <td>int pthread_join(pthread_t tid, void **rval_ptr);</td>
        <td>pthread.h</td>
        <td>获取线程的退出状态。</td>
    </tr>
    <tr>
        <td>6</td>
        <td>int pthread_cancel(pthread_t tid);</td>
        <td>pthread.h</td>
        <td>请求取消线程。</td>
    </tr>
    <tr>
        <td>7</td>
        <td>void pthread_clenup_push(void (*rtn)(void*), void *arg);</td>
        <td>pthread.h</td>
        <td>注册线程清理处理程序。</td>
    </tr>
    <tr>
        <td>8</td>
        <td>void pthread_cleanup_pop(int execute);</td>
        <td>pthread.h</td>
        <td>删除线程清理处理程序。</td>
    </tr>
    <tr>
        <td>9</td>
        <td>int pthread_detach(pthread_t tid);</td>
        <td>pthread.h</td>
        <td>分离线程。</td>
    </tr>
    <tr>
        <th colspan="4">互斥量原语</th>
    </tr>
    <tr>
        <td>10</td>
        <td>int pthread_mutex_init(pthread_mutex_t *mutex,<br>
            const struct pthread_mutexattr_t *attr);</td>
        <td>pthread.h</td>
        <td>初始化互斥量。</td>
    </tr>
    <tr>
        <td>11</td>
        <td>int pthread_mutex_destroy(pthread_mutex_t *mutex);</td>
        <td>pthread.h</td>
        <td>销毁互斥量。</td>
    </tr>
    <tr>
        <td>12</td>
        <td>int pthread_mutex_unlock(pthread_mutex_t *mutex);</td>
        <td>pthread.h</td>
        <td>解锁</td>
    </tr>
    <tr>
        <td>13</td>
        <td>int pthread_mutex_lock(pthread_mutex_t *mutex);</td>
        <td>pthread.h</td>
        <td>加锁。</td>
    </tr>
    <tr>
        <td>14</td>
        <td>int pthread_mutex_trylock(pthread_mutex_t *mutex);</td>
        <td>pthread.h</td>
        <td>尝试加锁。</td>
    </tr>
    <tr>
        <td>15</td>
        <td>int pthread_mutex_timedlock(pthread_mutex_t *mutex,<br>
            const struct timespec *tsptr);</td>
        <td>pthread.h<br>time.h</td>
        <td>在指定的时间之前加锁。</td>
    </tr>
    <tr>
        <th colspan="4">读写锁原语</th>
    </tr>
    <tr>
        <td>16</td>
        <td>int pthread_rwlock_init(pthread_rwlock_t *rwlock<br>
            pthread_rwlockattr_t *attr);</td>
        <td>pthread.h</td>
        <td>初始化读写锁。</td>
    </tr>
    <tr>
        <td>17</td>
        <td>pthread_rwlock_destroy(pthread_rwlock_t *rwlock);</td>
        <td>pthread.h</td>
        <td>销毁读写锁。</td>
    </tr>
    <tr>
        <td>18</td>
        <td>int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);</td>
        <td>pthread.h</td>
        <td>释放读写锁。</td>
    </tr>
    <tr>
        <td>19</td>
        <td>int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);</td>
        <td>pthread.h</td>
        <td>读加锁。</td>
    </tr>
    <tr>
        <td>20</td>
        <td>int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);</td>
        <td>pthread.h</td>
        <td>写加锁。</td>
    </tr>
    <tr>
        <td>21</td>
        <td>int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);</td>
        <td>pthread.h</td>
        <td>尝试读加锁。</td>
    </tr>
    <tr>
        <td>22</td>
        <td>int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);</td>
        <td>pthread.h</td>
        <td>尝试写加锁。</td>
    </tr>
    <tr>
        <td>23</td>
        <td>int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock);</td>
        <td>thread.h</td>
        <td>在指定的时间之前读加锁。</td>
    </tr>
    <tr>
        <td>24</td>
        <td>int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock);</td>
        <td>pthread.h</td>
        <td>在指定的时间之前写加锁。</td>
    </tr>
    <tr>
        <th colspan="4">条件变量原语</th>
    </tr>
    <tr>
        <td>25</td>
        <td>int pthread_cond_init(pthread_cond_t *cond,<br>
            pthread_condattr_t *attr);</td>
        <td>pthread.h</td>
        <td>初始化条件变量</td>
    </tr>
    <tr>
        <td>26</td>
        <td>int pthread_cond_destroy(pthread_cond_t *cond);</td>
        <td>pthread.h</td>
        <td>销毁条件变量。</td>
    </tr>
    <tr>
        <td>27</td>
        <td>int pthread_cond_wait(pthread_cond_t *cond,<br>
            pthread_mutex_t *mutex);</td>
        <td>pthread.h</td>
        <td>等待条件变量为真。</td>
    </tr>
    <tr>
        <td>28</td>
        <td>int pthread_cond_tindwait(pthread_cond_t *cond,<br>
            pthread_mutex_t *mutex,<br>
            const struct timespec *tsptr);</td>
        <td>pthread.h<br>time.h</td>
        <td>在规定的时间内等待条件变量为真。</td>
    </tr>
    <tr>
        <td>29</td>
        <td>int pthread_cond_signal(pthread_cond_t *cond);</td>
        <td>pthread.h</td>
        <td>唤醒至少一个等待该条件的线程。</td>
    </tr>
    <tr>
        <td>30</td>
        <td>int pthread_cond_broadcast(pthread_cond_t *cond);</td>
        <td>pthread.h</td>
        <td>唤醒所有等待该条件的线程。</td>
    </tr>
    <tr>
        <th colspan="4">自旋锁原语</th>
    </tr>
    <tr>
        <td>31</td>
        <td>int pthread_spin_init(pthread_spinlock_t *lock, int pshread);</td>
        <td>pthread.h</td>
        <td>初始化自旋锁</td>
    </tr>
    <tr>
        <td>32</td>
        <td>int pthread_spin_destroy(pthread_spinlock_t *lock);</td>
        <td>pthread.h</td>
        <td>销毁自旋锁</td>
    </tr>
    <tr>
        <td>33</td>
        <td>int pthread_spin_lock(pthread_spinlock_t *lock);</td>
        <td>pthread.h</td>
        <td>锁定自旋锁</td>
    </tr>
    <tr>
        <td>34</td>
        <td>int pthread_spin_trylock(pthread_spinlock_t *lock);</td>
        <td>pthread.h</td>
        <td>尝试锁定自旋锁</td>
    </tr>
    <tr>
        <td>35</td>
        <td>int pthread_spin_unlock(pthread_spinlock_t *lock);</td>
        <td>pthread.h</td>
        <td>解锁自旋锁</td>
    </tr>
    <tr>
        <th colspan="4">自旋锁原语</th>
    </tr>
    <tr>
        <td>36</td>
        <td>int pthread_barrier_init(pthread_barrier_t *barrier,<br>
            pthread_barrierattr_t *attr, unsigned int count);</td>
        <td>pthread.h</td>
        <td>初始化屏障。</td>
    </tr>
    <tr>
        <td>37</td>
        <td>int pthread_barrier_destroy(pthread_barrier_t *barrier);</td>
        <td>pthread.h</td>
        <td>反初始化屏障。</td>
    </tr>
    <tr>
        <td>38</td>
        <td>int pthread_barrier_wait(pthread_barrier_t *barrier);</td>
        <td>pthread.h</td>
        <td>屏障计数加1，并等待屏障计数达到要求。</td>
    </tr>
</table>