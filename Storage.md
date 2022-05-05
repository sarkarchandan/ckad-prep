# [Storage Options](https://kubernetes.io/docs/concepts/storage/)

Pods being an ephemeral entity we need to think about the possibility of data persistence. In K8s that comes in the form of several storage options, especially Volumes. Volumes can be used for storing and using the data or state of the Pods and Containers. A Pod can have multiple Volumes attached to it. We'd learn about something called `mountPath` to access the Volumes from the Pods. In general K8s supports the following storage options.

- [Volumes](https://kubernetes.io/docs/concepts/storage/volumes/)
- [PersistentVolumes and PersistentVolumeClaims](https://kubernetes.io/docs/concepts/storage/persistent-volumes/)
- [StorageClasses](https://kubernetes.io/docs/concepts/storage/storage-classes/)

## [Volumes](https://kubernetes.io/docs/concepts/storage/volumes/)

Volumes in K8s are similar to that from the Docker and they must have unique names. They are attached to Pod and may or may not be tied to lifetime of the Pod depending on the type of the Volume. A Volume mount references the Volume by name and defines a `mountPath`. Following are some basic types of Volumes storage options, that we'd like to start with.

- `emptyDir` - Tied to the Pod's lifetime and used for storing transient data. It is usually used for storing some data among the Containers running in a Pod.
- `hostPath` - Pods mount into the host Node's file system. If there is a possibility for the Node getting out of scope the data is lost.
- `nfs` - A network file system share can be mounted into the Pod.
- `configMap/secret` - Although we'd learn about the ConfigMaps as concrete K8s objects later, they are a type of Volumes.
- [persistentVolumeClaims](https://kubernetes.io/docs/concepts/storage/persistent-volumes/#persistentvolumeclaims) - A PersistentVolumeClaim (PVC) is a request for storage by a user. It is similar to a Pod. Pods consume node resources and PVCs consume PersistentVolumes resources. Pods can request specific levels of resources (CPU and Memory). Claims can request specific size and [access modes](https://kubernetes.io/docs/concepts/storage/persistent-volumes/#access-modes).
- `Cloud` - It refers to a cluster-wide storage and big topic of its own, which we'd explore in due time.

Above list is by no means an exhaustive one for Volume types, which K8s supports. Often it comes down to the decision from the organization on, which type of Volume would be used in the cluster setup. But we can at least start with some basic types to have some understanding on, how they fit in to the declarative way of defining resources in K8s.

> An example of the `emptyDir` Volume. This is not an ideal example, rather it is intended to get the basic idea, of how we make use of Volumes in a declarative manner. This type of Volume has the lifetime of the Pod itself.

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: my-nginx
  labels:
    name: my-nginx
spec:
  volumes: # Defines a Volume
    - name: html # Defines initial Volume name as html
      emptyDir: {} # It is an empty directory having the lifetime of the Pod itself
  containers:
    - name: my-nginx
      image: nginx:alpine
      volumeMounts: # Mounts to the defined Volume at the Container level
        - mountPath: /usr/share/nginx/html # Defines a mountPath referring to the html volume
          name: html
          readOnly: true
      resources:
        limits:
          memory: "128Mi"
          cpu: "200m"
      ports:
        - containerPort: 80
    - name: html-updater
      image: alpine
      command: ["/bin/sh", "-c"]
      args:
        - while true; do date >> /html/index.html; sleep 10; done # Defines an update schedule for a file
      volumeMounts: # Mounts to the defined Volume at the Container level
        - mountPath: /html # Also defines a mountPath referring to the same html volume
          name: html
      resources:
        limits:
          memory: "128Mi"
          cpu: "200m"
```

> An example of the `hostPath` Volume. This is not an ideal example, rather it is intended to get the basic idea, of how we make use of Volumes in a declarative manner. This type of Volume has the lifetime of the Node, in which the Pod is hosted.

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: docker
  labels:
    name: docker
spec:
  volumes: # Defines a Volume
    - name: docker-socket
      hostPath: # Specify, that the Volume is of type host path i.e. at the Node level
        path: /var/run/docker.sock
        type: Socket # It is socket type reference to get hold of the Docker daemon running at the Node level
  containers:
    - name: docker
      image: docker
      command: ["sleep"] # We could run Docker command at this point referring to the Docker daemon of the host if we want it
      args: ["1000"]
      resources:
        limits:
          memory: "128Mi"
          cpu: "200m"
      volumeMounts: # Mounts to the Volume
        - mountPath: /var/run/docker.sock
          name: docker-socket
```

> Example of the Cloud Volumes. These type of Volumes are much more reliable. They most often depend on, what kind of purpose we want to serve and the decision of the organization. Some examples of cloud volume providers are, Azure (Azure Disk and Azure File), AWS (Elastic Block Store), GCP (GCE PErsistent Disk) etc..

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: my-pod
  labels:
    name: my-pod
spec:
  volumes:
    - name: data
      azureFile:
        secretName: <azure-secret>
        shareName: <share-name>
        readOnly: false
  containers:
    - name: my-pod
      image: my-image
      resources:
        limits:
          memory: "128Mi"
          cpu: "200m"
      volumeMounts:
        - mountPath: /data/storage
          name: data
```

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: my-pod
  labels:
    name: my-pod
spec:
  volumes:
    - name: data
      awsElasticBlockStore:
        volumeID: <volume-id>
        fsType: ext4
  containers:
    - name: my-pod
      image: my-image
      resources:
        limits:
          memory: "128Mi"
          cpu: "200m"
      volumeMounts:
        - mountPath: /data/storage
          name: data
```

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: my-pod
  labels:
    name: my-pod
spec:
  volumes:
    - name: data
      gcePersistentDisk:
        pdName: data-storage
        fsType: ext4
  containers:
    - name: my-pod
      image: my-image
      resources:
        limits:
          memory: "128Mi"
          cpu: "200m"
      volumeMounts:
        - mountPath: /data/storage
          name: data
```

Before going ahead with other concepts let's see some of these basic Volume types in action.

```bash
# We are running the same emptyDir example, that we saw above. We have an html reader Container and an writer Container. So the writer writes the current date time stamp on the html file and the reader reads the html file.

$ k create -f fundamentals/Storage/vol_nginx_empty_dir.yaml --dry-run=client --validate=true
pod/nginx-alpine-volume created (dry run)

$ k apply -f fundamentals/Storage/vol_nginx_empty_dir.yaml
pod/nginx-alpine-volume created

$ k get po

NAME                  READY   STATUS    RESTARTS   AGE
nginx-alpine-volume   2/2     Running   0          22s

$ k port-forward pod/nginx-alpine-volume 8080:80
Forwarding from 127.0.0.1:8080 -> 80
Forwarding from [::1]:8080 -> 80

# And after some weight of few seconds now we see following output upon refreshing the browser window periodically.

# Thu May 5 05:24:30 UTC 2022 Thu May 5 05:24:40 UTC 2022 Thu May 5 05:24:50 UTC 2022 Thu May 5 05:25:00 UTC 2022 Thu May 5 05:25:10 UTC 2022 Thu May 5 05:25:20 UTC 2022 Thu May 5 05:25:30 UTC 2022 Thu May 5 05:25:40 UTC 2022 Thu May 5 05:25:50 UTC 2022 Thu May 5 05:26:00 UTC 2022 Thu May 5 05:26:10 UTC 2022
```

Moving on with the host path Volume example.

```bash
# We are using the same example of the host path Volume as above, where we have declared a Socket type Volume. Some other possibilities for the same are,
# DirectoryOrCreate
# Directory
# FileOrCreate
# File
# Socket
# CharDevice
# BlockDevice

$ k create -f fundamentals/Storage/vol_host_path_example.yaml --dry-run=client --validate=true
pod/docker created (dry run)

$ k apply -f fundamentals/Storage/vol_host_path_example.yaml
pod/docker created

$ k describe po/docker

...
Volumes:
  docker-socket:
    Type:          HostPath (bare host directory volume)
    Path:          /var/run/docker.sock
    HostPathType:  Socket
...
# And here we see somewhere down in the descriptive Pod specification the details of the Volume. Ideally, the Container has now got hold of the Docker daemon of the Node and we can run some Docker commands from the inside of the Container.

$ k exec po/docker -it -- sh

> docker --version
Docker version 20.10.14, build a224086

> docker ps -a
CONTAINER ID   IMAGE                          COMMAND                  CREATED          STATUS                      PORTS     NAMES
b5632fab570d   docker                         "sleep 1000"             4 minutes ago    Up 4 minutes                          k8s_docker_docker_default_9538a7f5-ac14-4ec8-9c20-9c25281b2f6a_0
c1ac30c6c864   k8s.gcr.io/pause:3.5           "/pause"                 4 minutes ago    Up 4 minutes                          k8s_POD_docker_default_9538a7f5-ac14-4ec8-9c20-9c25281b2f6a_0
85223d453c5e   2edf9c994f19                   "/kube-vpnkit-forwar…"   15 minutes ago   Up 15 minutes                         k8s_vpnkit-controller_vpnkit-controller_kube-system_8b5f58a4-b540-4cda-89a0-e2a1070cac21_404
...

> docker image ls
REPOSITORY                                                                   TAG                                                     IMAGE ID       CREATED         SIZE
docker.wdf.sap.corp:51022/com.sap.datahub.linuxx86_64/rbase                  3.2.10                                                  85475bcdf665   20 hours ago    2.64GB
docker.wdf.sap.corp:51022/com.sap.datahub.linuxx86_64/rbase                  3.2.9                                                   f0b7c6c16a61   7 days ago      2.63GB
docker.wdf.sap.corp:51022/com.sap.datahub.linuxx86_64/rbase                  2210.2.0                                                40678a030cf3   3 weeks ago     2.19GB
docker.wdf.sap.corp:51022/com.sap.datahub.linuxx86_64/rbase4                 2210.2.0                                                5721a6424510   3 weeks ago     2.21GB
...

# Now we are convinced, that we are really talking to the Docker daemon of the Node, which in this case is the same one of the host.

# As we understood, we can have host path Volumes referring to Directory and File in the Node (which in this case is the same as the host). So we have to understand, that if we go with host path volume as a demonstration in this case the directory / files would be stored at our host machine but their lifetime is really tied to the Node. Hence in a real cluster setup we have to make an educated decision.
```
