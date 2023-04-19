Projetos 1 e 2: Desenvolva programas para o uso de sockets na comunicação cliente com
- servidor concorrente sobre TCP (projeto 1), e
- servidor iterativo sobre UDP (projeto 2).
usando um objeto servidor de perfis (pessoas) que suporta múltiplos usuários. O servidor
armazena as seguintes informações sobre os perfis: email, nome, sobrenome, cidade de
residência, formação acadêmica, ano de formatura e habilidades. Um exemplo de perfil é
mostrado ao final deste documento.
Pelo menos as seguintes operações devem ser realizadas por um usuário (cliente):
• cadastrar um novo perfil utilizando o email como identificador;
• listar todas as pessoas (email e nome) formadas em um determinado curso;
• listar todas as pessoas (email e nome) que possuam uma determinada habilidade;
• listar todas as pessoas (email, nome e curso) formadas em um determinado ano;
• listar todas as informações de todos os perfis;
• dado o email de um perfil, retornar suas informações;
• remover um perfil a partir de seu identificador (email);
• fazer o download da imagem de um perfil a partir de seu identificador (somente no projeto
2, com UDP).
Considere que o servidor será administrado por um voluntário na universidade na qual as
pessoas se formaram, o qual será o único usuário a realizar operações de escrita (inserção,
e remoção de perfis). Não é necessário que o sistema faça autenticação de usuários através
de senhas ou outros mecanismos.
As habilidades poderão ser manipuladas como strings, podendo-se considerar que uma
pessoa possua uma habilidade buscada se esta for uma substring de suas habilidades. Os
dados deverão ser armazenados pelo servidor como arquivos. Nos testes a serem
realizados, deverão ser cadastrados pelo menos 5 perfis. Sugere-se, mas não é obrigatório,
o uso de máquinas diferentes para cliente e servidor.
Entregar:
• Os programas desenvolvidos em C. Devem estar bem estruturados e com comentários
descrevendo os procedimentos. Devem permitir a compilação e execução.

• Relatório. Deve conter introdução, descrição geral e casos de uso, armazenamento e
estruturas de dados, detalhes de implementação, e conclusão. Para o projeto 2, manter
a estrutura do trabalho 1, acrescido de uma seção com a comparação entre o software
desenvolvido nos dois projetos em termos de tamanho de código, confiabilidade, e nível
de abstração.
Datas de entrega: Projeto 1: 26 de abril; Projeto 2: 17 de maio.

Exemplo de perfil:
Email: maria_souza@gmail.com
Nome: Maria Sobrenome: Souza
Residência: Campinas
Formação Acadêmica: Ciência da Computação
Ano de Formatura: 2018
Habilidades: Ciência dos Dados, Internet das Coisas, Computação em Nuvem