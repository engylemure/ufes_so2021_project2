# task_a

Este programa tem como objetivo expor a implementacao de monitor ao utilizar `mutex` como o mecanismo de sincronizacao de processo/thread mais especificamente para lidar com o problema de gerenciamento de atendimento de um Pethop, o programa disponibiliza alguns argumentos para configurar a sua execucao dentre eles:

 - `loop` - especifica se as threads que representam um animal devem executar infinitamente. Valor padrao `true`.
 - `fans_size` - quantidade de torcedores a participarem do processo. Valor padrao 20.
 - `bathroom_size` - Quantidade de banheiros disponiveis no bar para os torcedores. Valor padrao 3.
 - `time_between_fans_arrival` - intervalo de chegada entre cada um dos torcedores. Valor padrao 1 (segundo).
 - `time_inside_bathroom` - Tempo que um torcedor ira gastar dentro do banheiro. Valor padrao 3 (segundos).
 - `first_team` - Time do primeiro torcedor a chegar no banheiro tendo como argumentos os valores `v` ou `vasco` para selecionar o vasco como primeiro. Valor padrao `flamengo`.

 
 exemplo: 

 ```bash
 ./make rebuild
 ./target/task_a loop=false bathroom_size=5 first_team=vasco
 ```

