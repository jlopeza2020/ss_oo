void 
prplus(int a )
{
    printff(a +1)
}

void (*printsome)(int a );

printsome = prplus;
printsome(1);
...
arreglar 