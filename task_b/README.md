# task_b

Este programa tem como objetivo expor a implementacao de monitor ao utilizar `semaphore` como o mecanismo de sincronizacao de processo/thread mais especificamente para lidar com o problema de gerenciamento de atendimento de um Pethop, o programa disponibiliza alguns argumentos para configurar a sua execucao dentre eles:

 - `loop` - especifica se as threads que representam um animal devem executar infinitamente. Valor padrao `true`.
 - `amount_of_animals` - quantidade de animais a participarem do processo. Valor padrao 20.
 - `room_size` - tamanho da sala de servico utilizada para atendimento dos animais. Valor padrao 3.
 - `animals_arrival_period` - intervalo de chegada entre cada um dos animais. Valor padrao 1 (segundo).
 - `time_on_service` - tempo que um animal estara sendo atendido. Valor padrao 3 (segundos).
 - `first_animal` - primeiro animal a chegar no petshop sendo possivel especificar `c` ou `cat`. Valor padrao `dog`.

 
 exemplo: 

 ```bash
 ./target/task_b loop=false room_size=5 first_animal=cat
 ```

